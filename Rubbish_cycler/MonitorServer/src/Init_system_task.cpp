/*
 * =====================================================================================
 *
 *       Filename:  Init_system_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/08/13 15:01:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Init_system_task.h"
#include "Init_system_work_item.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "AgentRegister.h"
#include "Dispatcher.h"
#include "CS_Epoll.h"

InitSystemTask::InitSystemTask(uint32_t index , RULE_TYPE type , Init_info info)
    :Task(index) ,
    m_info(info) , 
    m_type(type) , 
    m_item(NULL)
{

}

InitSystemTask::~InitSystemTask()
{
    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }
}

int InitSystemTask::init(tr1::function<void (bool , RULE_TYPE)> ret_cb)
{
    m_callback_func = ret_cb;

    struct All_Init_info all_info;
    all_info.dup = m_info.dup;
    all_info.mod = m_info.mod;
    layout_all_bucket(all_info.rule_table);

    WorkItem *item = new InitSystemWorkItem(m_type , all_info);
    if(NULL == item)
    {
        LOG_ERROR("InitSystemTask::create init system work item error !");
        return -1;
    }
    item->init(std::tr1::bind(&InitSystemTask::after_init_db_rule , this , 
                std::tr1::placeholders::_1));

    Dispatcher::getInstance()->send_request(item);
    m_item = item;

    return 0;
}

void InitSystemTask::layout_all_bucket(std::vector<struct HashRule> &result_vec)
{
    int bucket_num = cs_pow(m_info.mod);
    int dup_num = m_info.dup;
    result_vec.clear();
    int next_index = 0;
    const int vec_size = m_info.all_ip_list.size();
    struct HashRule rule;
    memset(&rule , 0 , sizeof(struct HashRule));

    for(int i = 0 ; i < bucket_num ; ++ i)
    {
        rule.bucket_nr = i;
        for(int j = 0 ; j < MAX_IP_NUM ; ++ j)
        {
            if(j < dup_num)
            {
                //将一个桶的多个副本分配在IP相邻的节点上
                rule.ip[j] = (m_info.all_ip_list)[next_index % vec_size];
                ++ next_index;
            }
            else
                rule.ip[j] = INVALID_IP;
        }

        result_vec.push_back(rule);
    }
}

void InitSystemTask::after_init_db_rule(bool ret)
{
    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }

    if(!ret)
    {
        do_callback(false);
        return ;
    }
    
    if(init_memory_rule() < 0)
    {
        LOG_ERROR("InitSystemTask::init memory rule error !");
        do_callback(false);
        return ;
    }
    do_callback(true);
}

void InitSystemTask::do_callback(bool ret)
{
    //执行回调的时候
    if(0 == m_father_agent_index)
    {
        m_callback_func(ret , m_type);
        return ;
    }
    if(AgentRegister::getInstance()->get_agent_from_index(m_father_agent_index) == NULL)
    {
        LOG_ERROR("InitSystemTask::opposite MPC Agent has been closed ! delete myself...");
        delete this;
        return ;
    }
    else 
    {
        m_callback_func(ret , m_type);
    }
}

int InitSystemTask::init_memory_rule()
{
    std::vector<struct HashRule> all_rule;
    layout_all_bucket(all_rule);

    RulerManager::getInstance()->init_system_rule(m_type);
    int size = all_rule.size();
    for(int i = 0 ; i < size ; ++ i)
    {
        RulerManager::getInstance()->add_new_hash_item(all_rule[i] , m_type);
    }

    RulerManager::getInstance()->set_current_mod(m_type , m_info.mod);

    return 0;
}
