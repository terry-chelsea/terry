/*
 * =====================================================================================
 *
 *       Filename:  Get_rule_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 14:58:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Get_rule_task.h"
#include "CSConfiguration.h"
#include "AgentRegister.h"
#include "CS_Epoll.h"
#include "SCAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"

#define FAKE_CS_IP "192.169.10.23"
#define FAKE_CS_PORT 16621

GetRuleTask::GetRuleTask(uint32_t agent_index , RULE_TYPE type)
    :Task(agent_index) ,
    m_rule_agent_index(0) ,
     m_type(type) 
{
    //这里可以使配置的选项，也可以是创建者提供的，这里使用全局的配置
    m_is_wait_client = false;
}

//无论如何在析构函数中唤醒所有等待的Agent，释放SCAgent
GetRuleTask::~GetRuleTask()
{
    if(m_rule_agent_index != 0)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_rule_agent_index);
        m_rule_agent_index = 0;
    }

    if(!m_wait_client_agent.empty())
    {
        wakeup_all_wait_request();
    }
}

//初始化工作，对于拉取规则这里需要创建一个SCAgent，然后执行Agent的初始化
//在初始化中会连接CS，然后返回等待着SCagent的回调，或者失败，或者读取到CS的回复
//在init函数中即使出错也不会执行回调函数，因此它的执行会返回到调用者的空间中

int GetRuleTask::init(std::tr1::function<void (bool)> ret_callback)
{
    //保存回调函数
    m_task_cb = ret_callback;
    
    SCAgent *agent = Task::create_new_agent(FAKE_CS_IP , FAKE_CS_PORT);

    if(NULL == agent)
    {
        LOG_ERROR("GetRuleTask::create SC agent error !");
        return -1;
    }

    //保存用户更新规则的SCAgent的句柄
    m_rule_agent_index = agent->get_agent_index();
    
    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    //根据当前Task的类型，更新MU或者SU的规则
    if(MU_RULER == m_type)
        header.cmd = MSG_RS_CS_UPDATE_MU_HASH;
    else
        header.cmd = MSG_RS_CS_UPDATE_SU_HASH;
    
    bool ret = false;
    std::string out_data;
    std::string out_string;
    cstore::pb_MSG_RS_CS_UPDATE_MU_HASH    pb_mu_hash;
    cstore::pb_MSG_RS_CS_UPDATE_SU_HASH    pb_su_hash;
    uint32_t current_version = RulerManager::getInstance()->get_current_version(m_type);

    if(MU_RULER == m_type)
    {
        pb_mu_hash.set_version(current_version);
        ret = ((!pb_mu_hash.IsInitialized() || (!pb_mu_hash.SerializeToString(&out_data))));
    }
    else 
    {
        pb_su_hash.set_version(current_version);
        ret = ((!pb_su_hash.IsInitialized() || (!pb_su_hash.SerializeToString(&out_data))));
    }

    //这里如果出现错误就需要释放掉创建的SCAgent了
    if(ret)
    {
        LOG_ERROR("GetRuleTask::protobuf Initialized error !");
        goto ERR;
    }
    //封装好需要发送的数据
    agent->do_package(header , out_data , out_string);

    //Agent执行初始化操作，这里需要传递三个参数，虽然最后一个参数本次不使用
    if(agent->init(std::tr1::bind(&GetRuleTask::done_get_new_rule , this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3 ) , out_string , NULL) < 0)
    {
        LOG_ERROR("GetRuleTask::agent init error ! ");
        goto ERR ;
    }
    
    return 0;
ERR :
    AgentRegister::getInstance()->delete_SC_agent(m_rule_agent_index);
    m_rule_agent_index = 0;

    return -1;
}

//回调函数，第三个参数为NULL，不使用
void GetRuleTask::done_get_new_rule(void *req , RET_TYPE type , void *data)
{
    MsgHeader header;
    InReq *cb_req = NULL;

    //如果SCAgent执行失败，不再执行接下来的操纵，回调上层的回调函数，
    //当然还可以使用其它的设置，例如重试多次等
    if(Task::parse_callback_ret(req , type) < 0)
    {
        goto ERR ;
    }

    cb_req = (InReq *)req;
    header = cb_req->m_msgHeader;
    //检查回复的报文的cmd字段是否符合定义，如果出错也不会执行接下来的操作
    if(MU_RULER == m_type && MSG_RS_CS_UPDATE_MU_HASH_ACK != header.cmd)
    {
        LOG_ERROR("GetRuleTask::Undefined mu ack header cmd : " + int_to_str(header.cmd));
        goto ERR ;
    }
    if(SU_RULER == m_type && MSG_RS_CS_UPDATE_SU_HASH_ACK != header.cmd)
    {
        LOG_ERROR("GetRuleTask::Undefined su ack header cmd : " + int_to_str(header.cmd));
        goto ERR ;
    }

    //这里需要保证原子性！
    if(get_new_rule_ack(cb_req) < 0)
    {
        LOG_ERROR("GetRuleTask::Parse new rule from CS error !");
        goto ERR ;
    }
    
    //每次更新报文之后都需要唤醒所有等待的客户端
    if(m_is_wait_client)
    {
        wakeup_all_wait_request();
    }
    //唤醒之后就可以执行回调了
    do_callback(true);

ERR :
    //这里可以使用其他的方式处理操作失败的情况，或者对不同的失败情况执行不同的操作
    //这里仅仅是回调上层执行失败
    if(m_is_wait_client)
    {
        wakeup_all_wait_request();
    }
    do_callback(false);
}

//根据回复更新RS的规则，这里需要保证更新的原子性
int GetRuleTask::get_new_rule_ack(InReq *req)
{
    MsgHeader header = req->m_msgHeader;
    std::string in_data(req->ioBuf , header.length);

    //如果RS已经是最新的规则了，那么这里就会返回空的负载数据
    if(0 == header.length)
    {
        LOG_EVENT("Get new rule from CS back , empty load data !");
        return 0;
    }

    cstore::pb_MSG_RS_CS_UPDATE_MU_HASH_ACK  in_mu_ack;
    cstore::pb_MSG_RS_CS_UPDATE_SU_HASH_ACK  in_su_ack;
    int sys_order_number = 0;
    std::list<cstore::Sys_Order>  sys_order_list;  
    
    //首先将所有的版本更新的信息保存在list中，再一一更新
    if(MU_RULER == m_type)
    {
        if(!in_mu_ack.ParseFromString(in_data))
        {
            LOG_ERROR("GetRuleTask::parse mu ack from protbuf error !");
            return -1;
        }
        sys_order_number = in_mu_ack.sys_order_size();
        for(int i = 0 ; i < sys_order_number ; ++ i)
        {
            sys_order_list.push_back(in_mu_ack.sys_order(i));
        }
    }
    else
    {
        if(!in_su_ack.ParseFromString(in_data))
        {
            LOG_ERROR("GetRuleTask::parse su ack from protbuf error !");
            return -1;
        }
        sys_order_number = in_su_ack.sys_order_size();
        for(int i = 0 ; i < sys_order_number ; ++ i)
        {
            sys_order_list.push_back(in_su_ack.sys_order(i));
        }
    }

    apply_all_new_rule(sys_order_list);

    return 0;
}

//这个函数全是内存的操作，因此可以返回为NULL，不考虑它的出错，因此只要能够执行到该函数
//一定能够更新规则表
void GetRuleTask::apply_all_new_rule(std::list<cstore::Sys_Order>& order_list)
{
    cstore::Sys_Order sys_order;
    int order_list_size = order_list.size();
    uint32_t order_cmd = 0;
    uint32_t hash_version = 0; 
    std::string order_content;

    if(0 == order_list_size)
    {
        LOG_EVENT("GET new rule , BUT empty order item !");
        return ;
    }
    std::list<cstore::Sys_Order>::iterator it = order_list.begin();
    std::list<cstore::Sys_Order>::iterator it_end = order_list.end();
    for( ; it != it_end ; ++ it)
    {
        order_cmd = it->order_header();
        hash_version = it->hash_version();
        order_content = it->order_content();

        deal_with_new_order_rule(order_cmd , order_content , hash_version);
    }

}

//对一个规则版本更新操作应用规则表上
void GetRuleTask::deal_with_new_order_rule(uint32_t cmd , std::string content , uint32_t version)
{
    OP_CMD  op_cmd;
    if(MU_RULER== m_type)
    {
        if(MSG_MPC_CS_MU_MIGRATE_BUCKET == cmd)
            op_cmd = MIGRATION;
        else if(MSG_MPC_CS_MU_EXTENT_BUCKET == cmd)
           op_cmd = EXTENT;
        else 
        {
            LOG_ERROR("GetRuleTask::Undefined cmd type !");
            return ;
        }
    } 
    else 
    {
        if(MSG_MPC_CS_SU_MIGRATE_BUCKET == cmd)
            op_cmd = MIGRATION;
        else if(MSG_MPC_CS_SU_EXTENT_BUCKET == cmd)
           op_cmd = EXTENT;
        else 
        {
            LOG_ERROR("GetRuleTask::Undefined cmd type !");
            return ;
        }
    }

    cstore::pb_MSG_MPC_CS_SU_MIGRATE_BUCKET  migration_pb;
    cstore::pb_MSG_MPC_CS_SU_EXTENT_BUCKET extent_pb;
    if(MIGRATION == op_cmd)
    {
        //需要保证这里不会出错
        if(!migration_pb.ParseFromString(content))
        {
            LOG_ERROR("GetRuleTask::parse protobuf from migration content error !");
            return ;
        }
        uint32_t mg_src_ip = migration_pb.source_ip();
        uint32_t mg_dst_ip = migration_pb.destination_ip();
        uint32_t mg_bucket_nr = migration_pb.bucket_num();

        RulerManager::getInstance()->new_migration_order(mg_bucket_nr , mg_src_ip 
                , mg_dst_ip , m_type);
    }
    else 
    {
        //需要保证这里不会出错
        if(!extent_pb.ParseFromString(content))
        {
            LOG_ERROR("GetRuleTask::parse protobuf from migration content error !");
            return ;
        }
//        uint32_t ex_new_mod = extent_pb.new_mod();

        //桶扩展的操作不会传递新的模值，每次自动增加1 
        RulerManager::getInstance()->new_extent_order(m_type);
    }
}

//唤醒所有等待请求更新额用户
void GetRuleTask::wakeup_all_wait_request()
{
    if(m_wait_client_agent.empty())
        return ;

    std::list<uint32_t>::iterator it = m_wait_client_agent.begin();
    std::list<uint32_t>::iterator it_end = m_wait_client_agent.end();

    SystemAgent *agent = NULL;
    CSTCPAgent  *cs_agent = NULL;
    uint32_t   agent_index = 0;
    for( ; it != it_end ; ++ it)
    {
        agent_index = *it;
        //如果对于的Agent已经被析构了，那么将不会执行这个Agent的回调了 
        cs_agent = AgentRegister::getInstance()->get_agent_from_index(agent_index);
        if(NULL == cs_agent)
            continue ;
        else
        {
            //使用动态转换操作，这里需要保证添加到该链表的都死SystemAgent类型
            agent = dynamic_cast<SystemAgent *>(cs_agent);
            if(NULL == agent)
            {
                LOG_ERROR("GetRuleTask::agent index " + int_to_str(agent_index) + " is not SystemAgent type !");
                continue ;
            }
        }

        //这是Agent同样统一提供的回调函数
        if(agent->after_get_new_rule() < 0)
        {
            LOG_ERROR("GetRuleTask::wake up agent : " + int_to_str(agent->get_agent_index()) + " get new rule error !");
        }
    }

    //所有的回调完成之后，需要清理对于的list
    m_wait_client_agent.clear();
}
