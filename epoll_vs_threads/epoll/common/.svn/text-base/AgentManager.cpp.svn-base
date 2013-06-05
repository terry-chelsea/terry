#include "AgentManager.h"
#include <iostream>
#include <string>
#include "Error.h"
#include "debug.h"
using namespace std;

AgentManager::AgentManager()
    :m_index(INIT_INDEX) , 
    m_list_agent_num(0)
{
}

AgentManager::~AgentManager()
{
    std::map<uint32_t , TCPAgent *>::iterator it = m_TCPAgentMap.begin();
    for( ; it != m_TCPAgentMap.end() ; ++ it)
    {
        TCPAgent *agent = it->second;
        agent->recycler();
        delete agent;
    }

    m_TCPAgentMap.clear();
}

TCPAgent *AgentManager::createAgent(Epoll * epl)
{
    TCPAgent *agent = new TCPAgent(epl);
    if(NULL == agent)
    {
        return NULL;
    }
    else 
    {
        agent->set_agent_index(generate_index());
        this->addAgent(agent);
        return agent;
    }
}

TCPAgent *AgentManager::createAgent(const SocketAddress &addr , Epoll *epl)
{
    TCPAgent *agent = new TCPAgent(addr , epl);
    if(NULL == agent)
    {
        return NULL;
    }
    else 
    {
        agent->set_agent_index(generate_index());
        this->addAgent(agent);
        return agent;
    }
}

TCPAgent *AgentManager::createAgent(const TCPSocket &sock , const SocketAddress &addr , 
        Epoll *epl)
{
    TCPAgent *agent = new TCPAgent(sock , addr , epl);
    if(NULL == agent)
    {
        return NULL;
    }
    else 
    {
        agent->set_agent_index(generate_index());
        this->addAgent(agent);
        LOG_INFO("Create agent of index %d success ..." , agent->get_agent_index());
        return agent;
    }
}

uint32_t AgentManager::generate_index()
{
    m_index ++;
    if(m_index == MAX_INDEX)
        m_index = INIT_INDEX;

    return m_index;
}

int AgentManager::addAgent(TCPAgent* agent)
{
    if(NULL == agent)
    {
        LOG_WARNING("add NULL to agent manager !");
        return -1;
    }

    uint32_t index = agent->get_agent_index();
    if(0 == index)
    {
        agent->set_agent_index(generate_index());
    }

    pair<map<uint32_t , TCPAgent*>::iterator, bool> ret;

    int try_time = 0;
    for(try_time = 0 ; try_time < TRY_TIMES ; ++ try_time)
    {
        ret = m_TCPAgentMap.insert(map<uint32_t , TCPAgent*>::value_type(index , agent));
        if(ret.second)
            break;
        else 
        {
            index = generate_index();
            agent->set_agent_index(index);
        }
    }

    if(TRY_TIMES == try_time)
    {
        LOG_ERROR("Add agent to manager map error !");
        return -1;
    }

    return 0;
}

void AgentManager::deleteAgent(uint32_t index)
{
    map<uint32_t , TCPAgent*>::iterator iter = m_TCPAgentMap.find(index);

    if(iter != m_TCPAgentMap.end())
    {
        m_TCPAgentMap.erase(iter);
    }
    else
    {
        LOG_WARNING("Can not find agent of index %u in manager !" , index);
    }
    LOG_INFO("Delete agent of index %d success ..." , index);
}

TCPAgent* AgentManager::getAgent(uint32_t index)
{
    TCPAgent* pAgent = NULL;
    map<uint32_t , TCPAgent*>::iterator iter = m_TCPAgentMap.find(index);
    if(iter != m_TCPAgentMap.end())
        pAgent = iter ->second;
    else
    {
        LOG_ERROR("can't find index %u !" , index);
        return NULL;
    }

    return pAgent;
}

int AgentManager::recyclerAgent(uint32_t index)
{
    TCPAgent *agent = getAgent(index);
    if(NULL == agent)
    {
        LOG_WARNING("Recycler agent of NULL ...");
        return 0;
    }

    int ret = agent->recycler();
    add_recycler_agent(agent);
    return ret;
}
int AgentManager::recyclerAgent(Agent *&agent)
{
    int ret = 0;
    TCPAgent *ag = dynamic_cast<TCPAgent *>(agent);
    if(ag != NULL)
    {
        ret = ag->recycler();
        int index = ag->get_agent_index();
        this->deleteAgent(index);
        add_recycler_agent(ag);
    }

    return ret;
}

int AgentManager::recyclerAgent(TCPAgent *&agent)
{
    int ret = 0;
    if(agent != NULL)
    {
        ret = agent->recycler();
        int index = agent->get_agent_index();
        this->deleteAgent(index);
        add_recycler_agent(agent);
    }

    return ret;
}

void AgentManager::add_recycler_agent(TCPAgent *&agent)
{
    this->m_recycler_list.push_back(agent);
    ++ m_list_agent_num;
}

int AgentManager::clean_agent()
{
    if(0 == m_list_agent_num)
        return 0;

    TCPAgent *agent = NULL;
    std::list<TCPAgent *>::iterator it = m_recycler_list.begin();
    for( ; it != m_recycler_list.end() ; ++ it)
    {
        agent = *it;
        delete agent;
    }

    m_recycler_list.clear();
    int ret = m_list_agent_num;
    m_list_agent_num = 0;

    return ret;
}
