/*
 * =====================================================================================
 *
 *       Filename:  AgentRegister.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 10:29:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "AgentRegister.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "SCAgent.h"
#include "RulerManager.h"
#include "SCAgent.h"

AgentRegister::AgentRegister()
    :m_next_agent_index(1) 
{
}

AgentRegister::~AgentRegister()
{
    std::map<uint32_t , CSTCPAgent *>::iterator it = m_normal_agent_map.begin();
    std::map<uint32_t , CSTCPAgent *>::iterator it_end = m_normal_agent_map.end();

    while(it != it_end)
    {
        CSTCPAgent *agent = it->second;
        agent->recycler();

        delete agent;

        ++it;
    }

    std::list<CSTCPAgent *>::iterator lit = m_special_agent_list.begin();
    std::list<CSTCPAgent *>::iterator lit_end = m_special_agent_list.end();
    while(lit != lit_end)
    {
        CSTCPAgent *agent = *lit;
        agent->recycler();

        delete agent;

        ++lit;
    }

    //recycler_SC_agent();

    std::list<CSTCPAgent *>::iterator rit = m_recycler_agent_list.begin();
    std::list<CSTCPAgent *>::iterator rit_end = m_recycler_agent_list.end();
    while(rit != rit_end)
    {
        CSTCPAgent *agent = *rit;
        agent->recycler();

        delete agent;
        ++ rit;
    }
}

//创建并返回一个SCAgent
SCAgent *AgentRegister::create_SC_agent(const SocketAddress &addr , Epoll *epl)
{
    SCAgent *agent = new SCAgent(addr , epl);
    
    return agent;
}

//将一个Agent放入到回收队列行，下次Epoll循环的时候再进行回收
void AgentRegister::delete_SC_agent(uint32_t agent_index)
{
    std::map<uint32_t , CSTCPAgent *>::iterator it_find = m_normal_agent_map.begin();

    it_find = m_normal_agent_map.find(agent_index);
    //如果没有找到，出错
    if(it_find == m_normal_agent_map.end())
    {
        LOG_ERROR("AgentRegister::can not find agent : " + int_to_str(agent_index) 
                + " in normal agent map !");

        return ;
    }
    CSTCPAgent *agent = it_find->second;
    m_recycler_agent_list.push_back(agent);
}

void AgentRegister::recycler_SC_agent()
{
    std::list<CSTCPAgent *>::iterator it = m_recycler_agent_list.begin();
    std::list<CSTCPAgent *>::iterator it_end = m_recycler_agent_list.end();

    for( ; it != it_end ; ++ it)
    {
        CSTCPAgent *agent = *it ;
        agent->recycler();

        delete agent;
    }

    m_recycler_agent_list.clear();
}

void AgentRegister::generate_index(uint32_t &index)
{
    index = m_next_agent_index;

    ++ m_next_agent_index;
}

uint32_t AgentRegister::add_agent(CSTCPAgent *agent)
{
    uint32_t index = 0;
    generate_index(index);

    m_normal_agent_map.insert(make_pair(index , agent));

    return index;
}

void AgentRegister::delete_agent(uint32_t index)
{
    map<uint32_t , CSTCPAgent *>::iterator it_find = m_normal_agent_map.begin();

    it_find = m_normal_agent_map.find(index);
    if(it_find != m_normal_agent_map.end())
    {
        m_normal_agent_map.erase(it_find);
    }
    else
    {
        LOG_ERROR("AgentRegister::can not find agent of index : " + int_to_str(index));
    }
}

CSTCPAgent *AgentRegister::get_agent_from_index(uint32_t index)
{
    std::map<uint32_t , CSTCPAgent *>::iterator it = m_normal_agent_map.begin();
    std::map<uint32_t , CSTCPAgent *>::iterator it_end = m_normal_agent_map.end();

    for( ; it != it_end ; ++ it)
    {
        if(it->first == index)
            break;
    }
    if(it != it_end)
        return it->second;
    else
    {
        LOG_EVENT("Agent index : " + int_to_str(index) + " has been deleted !");
        return NULL;
    }
}

void AgentRegister::add_special_agent(CSTCPAgent *agent)
{

}

void AgentRegister::delete_special_agent(uint32_t index)
{

}

