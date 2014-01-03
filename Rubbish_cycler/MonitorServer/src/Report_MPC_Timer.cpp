/*
 * =====================================================================================
 *
 *       Filename:  Report_MPC_Timer.cpp
 *
 *    Description:  定时向MPC发送信息
 *
 *        Version:  1.0
 *        Created:  11/06/12 18:15:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Report_MPC_Timer.h"
#include "RulerManager.h"
#include "CSConfiguration.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "AgentRegister.h"
#include "../common/protocol/protocol.h"
#include "CS_Epoll.h"

ReportMPCTimer::ReportMPCTimer(Epoll *epl , int sec , int nsec)
    :TimerAgent(epl , PERSIST_TIMER | CAN_REPEAT , sec , nsec) ,
    m_report_agent_index(0)
{

}

ReportMPCTimer::ReportMPCTimer(Epoll *epl)
    :TimerAgent(epl , PERSIST_TIMER | CAN_REPEAT)
{

}

ReportMPCTimer::~ReportMPCTimer()
{

}

int ReportMPCTimer::expire_action()
{
    std::string out_data;
    RulerManager *rule_ptr = RulerManager::getInstance();
    uint32_t  ip_addr = 0;
    struct Node_Info info;
    struct Bucket_Info bucket_info;

    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    cstore::pb_MSG_CS_MPC_REPORT_BUCKET_LIST   out_report_bucket;
    cstore::Report_Bucket_List                *report_bucket_list_ptr = NULL;
    cstore::Bucket_List                       *bucket_list_ptr = NULL;
    cstore::Bucket_Item                       *bucket_item_ptr = NULL;

    std::map<uint32_t , struct Node_Info>::iterator it;
    std::map<uint32_t , struct Node_Info>::iterator it_end;

    it = rule_ptr->m_mu_last_load.begin();
    it_end = rule_ptr->m_mu_last_load.end();
    for( ; it != it_end ; ++ it)
    {
        ip_addr = it->first;
        info = it->second;
        report_bucket_list_ptr = out_report_bucket.add_mu_bucket_list();
        bucket_list_ptr = report_bucket_list_ptr->mutable_node_bucket_list();

        report_bucket_list_ptr->set_node_storage_limit(info.storage_limit);
        bucket_list_ptr->set_mu_ip(ip_addr);

        for(uint32_t j = 0 ; j < info.bucket_list.size() ; ++ j)
        {
            bucket_info = info.bucket_list[j];
            bucket_item_ptr = bucket_list_ptr->add_bucket_item();
            bucket_item_ptr->set_bucket(bucket_info.bucket_nr);
            bucket_item_ptr->set_bucket_load(bucket_info.bucket_load);
        }
    }

    it = rule_ptr->m_su_last_load.begin();
    it_end = rule_ptr->m_su_last_load.end();
    for( ; it != it_end ; ++ it)
    {
        ip_addr = it->first;
        info = it->second;
        report_bucket_list_ptr = out_report_bucket.add_su_bucket_list();
        bucket_list_ptr = report_bucket_list_ptr->mutable_node_bucket_list();

        report_bucket_list_ptr->set_node_storage_limit(info.storage_limit);
        bucket_list_ptr->set_mu_ip(ip_addr);

        for(uint32_t j = 0 ; j < info.bucket_list.size() ; ++ j)
        {
            bucket_info = info.bucket_list[j];
            bucket_item_ptr = bucket_list_ptr->add_bucket_item();
            bucket_item_ptr->set_bucket(bucket_info.bucket_nr);
            bucket_item_ptr->set_bucket_load(bucket_info.bucket_load);
        }
    }
    
    if((!out_report_bucket.IsInitialized()) || (!out_report_bucket.SerializeToString(&out_data)))
    {
        LOG_ERROR("ReportMPCTimer::Initialized protobuf error !");
        return -1;
    }

    header.cmd = MSG_CS_MPC_REPORT_BUCKET_LIST;
    header.length = out_data.size();
    

    return send_data_and_callback(header , out_data);
}

int ReportMPCTimer::send_data_and_callback(MsgHeader &header , const std::string &out)
{
    SocketAddress mpc_addr(CSConfiguration::getInstance()->mpc_ip.c_str() , 
            CSConfiguration::getInstance()->mpc_to_port);
    SCAgent *agent = AgentRegister::getInstance()->create_SC_agent(mpc_addr
            , CSEpoll::getInstance());

    if(NULL == agent)
    {
        LOG_ERROR("RunControl::create SC agent error !");
        return -1;
    }
    m_report_agent_index = agent->get_agent_index();
    
    std::string req_data;
    agent->do_package(header , out , req_data);

    int ret = agent->init(std::tr1::bind(&ReportMPCTimer::done_send_message_to_mpc , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3) , req_data , NULL);
    if(ret < 0)
    {
        LOG_ERROR("ReportMPCTimer::SCagent init error !");
        AgentRegister::getInstance()->delete_SC_agent(m_report_agent_index);
        m_report_agent_index = 0;
        return -1;
    }

    return 0;
}

void ReportMPCTimer::done_send_message_to_mpc(void *req , RET_TYPE ret , void *data) 
{
    InReq *back_req = (InReq *)req;

    if(CONNECT_ERROR == ret)
    {
        LOG_ERROR("ReportMPCTimer::Connect to mpc error when report MPC !");
    }
    else if(WRITE_ERROR == ret)
    {
        LOG_ERROR("ReportMPCTimer::Write to mpc error when report MPC !");
    }
    else if(UNKOWN_ERROR == ret)
    {
        LOG_ERROR("ReportMPCTimer::Report MPC error : Unknow error !");
    }
    else if(SUCCESSFULLY == ret)
    {
        if(NULL == back_req)
        {
            LOG_ERROR("ReportMPCTimer::Report MPC : NO ACK !");
            return ;
        }
        else 
        {
            if(MSG_CS_MPC_REPORT_BUCKET_LIST_ACK != back_req->m_msgHeader.cmd)
            {
                LOG_ERROR("ReportMPCTimer::Report MPC : Unknown ACK type !");
            }
            else 
            {
                static uint32_t send_counter = 0;
                LOG_EVENT("Report Message to MPC cuccseefully ! Counter : " 
                        + int_to_str(++ send_counter));
            }
        }
    }

    return ;
}
