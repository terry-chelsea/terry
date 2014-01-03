/*
 * =====================================================================================
 *
 *       Filename:  HeartbeatAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 21:38:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Log_warp.h"
#include "Lib_tool.h"
#include "HeartbeatAgent.h"
#include "HB_manager.h"

HeartbeatAgent::HeartbeatAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(sock , addr , epl) ,
    m_heartbeat_counter(0) , 
    m_type(UNKNOWN_HEARTBEAT)
{

}

HeartbeatAgent::~HeartbeatAgent()
{

}

int HeartbeatAgent::init()
{
    m_time_stamp = time(NULL);

    HBManager::getInstance()->add_heartbeat_agent(this);

    LOG_EVENT("Heartbeat connection from IP : " + TCPAgent::m_Addr.ip +
            " AND Port : " + int_to_str(TCPAgent::m_Addr.port));

    return 0;
}

int HeartbeatAgent::recycler()
{
    HBManager::getInstance()->heartbeat_closed(this);
    LOG_EVENT("Heartbeat Closed from IP " + TCPAgent::m_Addr.ip  
           + " AND Port : "+ ushort_to_str(TCPAgent::m_Addr.port) + 
           " Node type is " + type_str[m_type]);
    
    if(TCPAgent::recycler() < 0)
    {
        LOG_ERROR("HeartbeatAgent::recycler agent error !");
        return -1;
    }

    return 0;
}

int HeartbeatAgent::destory()
{
    LOG_EVENT("Heartbeat Closed from IP " + TCPAgent::m_Addr.ip  
           + " AND Port : "+ ushort_to_str(TCPAgent::m_Addr.port) + 
           " Node type is " + type_str[m_type]);
    
    if(TCPAgent::recycler() < 0)
    {
        LOG_ERROR("HeartbeatAgent::recycler agent error !");
        return -1;
    }

    return 0;
}

void HeartbeatAgent::readBack(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t cmd = header.cmd;

    int ret = -1;
    switch(cmd)
    {
        case MSG_MU_CS_HEARTBEAT_HANDSHAKE :
        {
            ret = do_heartbeat_handshake(MU_HEARTBEAT , req);
            if(ret < 0)
            {
                LOG_ERROR("HeartbeatAgent::do mu heartbeat handshake error !");
            }
            break ;
        }
        case MSG_SU_CS_HEARTBEAT_HANDSHAKE :
        {
            ret = do_heartbeat_handshake(SU_HEARTBEAT , req);
            if(ret < 0)
            {
                LOG_ERROR("HeartbeatAgent::do su heartbeat handshake error !");
            }
            break ;
        }
        case MSG_MU_CS_HEARTBEAT :
        {
            ret = do_heartbeat(MU_HEARTBEAT , req);
            if(ret < 0)
            {
                LOG_ERROR("HeartbeatAgent::do mu heartbeat error !");
            }
            break ;
        }
        case MSG_SU_CS_HEARTBEAT :
        {
            ret = do_heartbeat(SU_HEARTBEAT , req);
            if(ret < 0)
            {
                LOG_ERROR("HeartbeatAgent::do su heartbeat error !");
            }
            break ;
        }
        case MSG_RS_CS_HEARTBEAT :
        {
            ret = do_heartbeat(RS_HEARTBEAT , req);
            if(ret < 0)
            {
                LOG_ERROR("HeartbeatAgent::do rs heartbeat error !");
            }
            break ;
        }

        default :
            LOG_ERROR("HeartbeatAgent::Undefined cmd type : " + int_to_str(cmd));
            break ;
    }
}

void HeartbeatAgent::update_time_stamp()
{
    time_t now = time(NULL);
    if((time_t)-1 == now)
        return ;

    m_time_stamp = now;
}

int HeartbeatAgent::do_heartbeat_handshake(HEARTBEAT_TYPE type , InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    std::string empty_data;
    std::string input_data(req.ioBuf , header.length);
    uint32_t storage_limit = 0;
    uint32_t cmd_ack = 0;

    cstore::pb_MSG_MU_CS_HEARTBEAT_HANDSHAKE  in_mu_handshake;
    cstore::pb_MSG_SU_CS_HEARTBEAT_HANDSHAKE  in_su_handshake;

    update_time_stamp();
    m_type = type;
    if(MU_HEARTBEAT == type)
    {
        cmd_ack = MSG_MU_CS_HEARTBEAT_HANDSHAKE_ACK;
        if(!in_mu_handshake.ParseFromString(input_data))
        {
            LOG_ERROR("HeartbeatAgent::parse protobuf of mu heartbeat handshake error !");
            goto ERR;
        }
        std::cerr<<in_mu_handshake.DebugString()<<std::endl<<std::endl;
        storage_limit = in_mu_handshake.storage_load_limit();
    }
    else if(SU_HEARTBEAT == type)
    {
        cmd_ack = MSG_SU_CS_HEARTBEAT_HANDSHAKE_ACK;
        if(!in_su_handshake.ParseFromString(input_data))
        {
            LOG_ERROR("HeartbeatAgent::parse protobuf of su heartbeat handshake error !");
            goto ERR;
        }
        std::cerr<<in_su_handshake.DebugString()<<std::endl<<std::endl;
        storage_limit = in_su_handshake.storage_load_limit();
    }

    std::cout<<"get heartbeat handshake type is "<<(type == SU_HEARTBEAT ? "SU !" : "MU!")<<endl;
   
    if(MU_HEARTBEAT == type)
        HBManager::getInstance()->init_node_heartbeat_info(MU_RULER , this , storage_limit);
    else if(SU_HEARTBEAT == type)
        HBManager::getInstance()->init_node_heartbeat_info(SU_RULER , this , storage_limit );
    

    CSTCPAgent::reply_request(cmd_ack , CS_OK , 0 , 0 , empty_data);
    return 0;

ERR :
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 0 , 0 , empty_data);
    return  -1;
}

int HeartbeatAgent::do_heartbeat(HEARTBEAT_TYPE type , InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    std::string input_data(req.ioBuf , header.length);
    std::string empty_data;
    uint32_t cmd_ack = 0;
    uint32_t local_ip = INVALID_IP;

    cstore::pb_MSG_MU_CS_HEARTBEAT   in_mu_heartbeat;
    cstore::pb_MSG_SU_CS_HEARTBEAT   in_su_heartbeat;
    cstore::Bucket_Item              bucket_item;
    std::list<cstore::Bucket_Item>   bucket_item_list;
    
    int bucket_num = 0;

    update_time_stamp();
    m_type = type;
    if(RS_HEARTBEAT == type)
    {
        CSTCPAgent::reply_request(MSG_RS_CS_HEARTBEAT_ACK , CS_OK , 0 , 0 , empty_data);
        return 0;
    }

    if(MU_HEARTBEAT == type)
    {
        cmd_ack = MSG_MU_CS_HEARTBEAT_ACK;
        if(!in_mu_heartbeat.ParseFromString(input_data))
        {
            LOG_ERROR("HeartbeatAgent::parse protobuf of mu heartbeat error !");
            goto ERR;
        }
//        std::cerr<<in_mu_heartbeat.DebugString()<<std::endl<<std::endl;;

        bucket_num = in_mu_heartbeat.bucket_item_size();
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            bucket_item = in_mu_heartbeat.bucket_item(i);
            bucket_item_list.push_back(bucket_item);
        }
    }
    else if(SU_HEARTBEAT == type)
    {
        cmd_ack = MSG_SU_CS_HEARTBEAT_ACK;
        if(!in_su_heartbeat.ParseFromString(input_data))
        {
            LOG_ERROR("HeartbeatAgent::parse protobuf of mu heartbeat error !");
            goto ERR;
        }
//        std::cerr<<in_su_heartbeat.DebugString()<<std::endl<<std::endl;;

        bucket_num = in_su_heartbeat.bucket_item_size();
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            bucket_item = in_su_heartbeat.bucket_item(i);
            bucket_item_list.push_back(bucket_item);
        }
    }
    if(string_to_int_ip(m_Addr.getIP() , local_ip) < 0)
    {
        LOG_ERROR("HeartbeatAgent::change int ip to string error !");
        goto ERR;
    }

    if(MU_HEARTBEAT == type)
        update_last_bucket_load(MU_RULER , local_ip , bucket_item_list);
    else if(SU_HEARTBEAT == type)
        update_last_bucket_load(SU_RULER , local_ip , bucket_item_list);

    CSTCPAgent::reply_request(cmd_ack , CS_OK , 0 , 0 , empty_data);
    return 0;

ERR :
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 0 , 0 , empty_data);
    return -1;
}

void HeartbeatAgent::update_last_bucket_load(RULE_TYPE type , uint32_t ip , 
        std::list<cstore::Bucket_Item> &bucket_list)
{
    struct Bucket_Info info;
    memset(&info , 0 , sizeof(info));
    std::vector<struct Bucket_Info> bucket_vec;
    
    std::list<cstore::Bucket_Item>::iterator bucket_list_it = bucket_list.begin();
    std::list<cstore::Bucket_Item>::iterator bucket_list_end = bucket_list.end();

    while(bucket_list_it != bucket_list_end)
    {
        info.bucket_nr = bucket_list_it->bucket();
        info.bucket_load = bucket_list_it->bucket_load();
        bucket_vec.push_back(info);

        ++ bucket_list_it;
    }
    
    HBManager::getInstance()->update_last_bucket_load(type , this , bucket_vec);
}
