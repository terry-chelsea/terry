
#include "Epoll.h"
#include "TCPAgent.h"
#include "debug.h"
#include "Client.h"
#include "AgentManager.h"

Client::Client(Epoll * epl)
    :m_addr()  ,
    m_cb_flag(false) ,
    m_ret(0)
{
    m_agent = AgentManager::getInstancePtr()->createAgent(epl);
    m_ops = default_operation;
}

Client::Client(Epoll *epl , const SocketAddress &addr)
    :m_addr(addr.getIP() , addr.getPort()) 
{
    m_agent = AgentManager::getInstancePtr()->createAgent(epl);
    m_ops = default_operation;
}

Client::~Client()
{
    if(m_agent != NULL)
        AgentManager::getInstancePtr()->recyclerAgent(m_agent);

    if(m_cb_flag)
        m_cb(m_ret);

    m_agent = NULL;
}

int Client::connect(const SocketAddress &addr)
{
    m_addr = addr;

    return this->connect();
}


int Client::connect()
{
    if(NULL == m_agent)
    {
        LOG_WARNING("Agent is not initialized in Client !");
        return 0;
    }

    m_agent->set_agent_operations(m_ops);
    int ret = m_agent->connect(m_addr);
    if(ret < 0)
    {
        LOG_ERROR("Client connect to ip %s and port %d error !" , 
                m_addr.getIP() , m_addr.getPort());
        return -1;
    }

    return 0;
}

void Client::set_operations(const struct Agent_operations &op)
{
    m_ops = op;
    if(m_agent != NULL)
        m_agent->set_agent_operations(m_ops);
}

void Client::set_callback(std::tr1::function<void (int)> cb)
{
    m_cb = cb;
    m_cb_flag = cb;
}


