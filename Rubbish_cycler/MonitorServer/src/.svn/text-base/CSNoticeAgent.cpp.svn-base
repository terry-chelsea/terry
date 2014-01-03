/*
 * =====================================================================================
 *
 *       Filename:  CSNoticeAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/12 01:03:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "CSNoticeAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "Get_rule_task.h"
#include "Task_manager.h"
#include "../common/protocol/protocol.h"

CSNoticeAgent::CSNoticeAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(sock , addr , epl)
{

}

CSNoticeAgent::~CSNoticeAgent()
{

}

int CSNoticeAgent::init()
{
    return 0;
}

void CSNoticeAgent::readBack(InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t cmd = header.cmd;

    int ret = -1;
    switch(cmd)
    {
        case MSG_CS_RS_RELOAD_MU_HASH :
        {
            m_rule_type = MU_RULER;
            ret = get_new_rule();
            if(ret < 0)
            {
                LOG_ERROR("CSNoticeAgent::get new mu rule error !");
            }
            break ;
        }
        
        case MSG_CS_RS_RELOAD_SU_HASH :
        {
            m_rule_type = MU_RULER;
            ret = get_new_rule();
            if(ret < 0)
            {
                LOG_ERROR("CSNoticeAgent::get new su rule error !");
            }
            break ;
        }
        
        default :
            LOG_ERROR("CSNoticeAgent::undefined cmd type : " + int_to_str(cmd));
            break ;
    }
}

int CSNoticeAgent::get_new_rule()
{
    TaskManager *task_manager = TaskManager::getInstance();
    GetRuleTask *rule_task = NULL;
    uint32_t ack_cmd = 0;
    int ret = -1;

    //当前没有正在更新规则的请求在执行，创建
    if(!task_manager->RS_is_getting_rule(m_rule_type))
    {
        rule_task = task_manager->create_get_rule_task(m_rule_type , CSTCPAgent::m_index);
        if(MU_RULER == m_rule_type)
        {
            ack_cmd = MSG_CS_RS_RELOAD_MU_HASH_ACK;
            ret = rule_task->init(std::tr1::bind(&CSNoticeAgent::done_get_new_rule
                            , this 
                            , std::tr1::placeholders::_1));
        }
        else
        {
            ack_cmd = MSG_CS_RS_RELOAD_SU_HASH_ACK;
            ret = rule_task->init(std::tr1::bind(&CSNoticeAgent::done_get_new_rule
                            , this 
                            , std::tr1::placeholders::_1));
        }
        if(ret < 0)
        {
            LOG_ERROR("CSNoticeAgent::get su or mu rule task init error !");
            task_manager->delete_get_rule_task(m_rule_type);
            goto ERR ;
        }
    }
    else
    {
        task_manager->append_new_update_request(m_rule_type , this);
    }

    return 0;

ERR :
    std::string empty_data;
    CSTCPAgent::reply_request(ack_cmd , RS_ERROR , 0 , 0 , empty_data);

    return -1;
}

void CSNoticeAgent::done_get_new_rule(bool ret)
{
    TaskManager *task_manager = TaskManager::getInstance();
    CSNoticeAgent *agent = NULL;
    std::string empty_str;
    uint32_t ack_cmd = 0;

    task_manager->delete_get_rule_task(m_rule_type);

    if(MU_RULER == m_rule_type)
        ack_cmd = MSG_CS_RS_RELOAD_MU_HASH_ACK ;
    else
        ack_cmd = MSG_CS_RS_RELOAD_SU_HASH_ACK ;

    if(!ret) 
    {
        LOG_ERROR("CSNoticeAgent::get rule error !");
        CSTCPAgent::reply_request(ack_cmd , 
                RS_UPDATE_ERROR , 0 , 0 , empty_str);

        set_dead_line("Update rule from CS error ! please restart and Update all rule from CS !");
        die();
    }
    else
    {
        CSTCPAgent::reply_request(ack_cmd , 
                RS_OK , 0 , 0 , empty_str);

        while(task_manager->RS_need_continue_update_rule(m_rule_type))
        {
            agent = task_manager->delete_first_update_request(m_rule_type);
            if(agent->get_new_rule() < 0)
            {
                LOG_ERROR("CSNoticeAgent::get mu rule task init error !");
                continue ;
            }
            else 
                break ;
        }

    }
    return ;
}
