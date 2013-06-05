/*
 * =====================================================================================
 *
 *       Filename:  TCPListenAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/13 22:12:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "TCPListenAgent.h"
#include "debug.h"
#include "Error.h"
#include "Agent.h"
#include "AgentManager.h"
#include <netinet/tcp.h>
#include <iostream>
#include <cstring>
#include <errno.h>

//initialize the TCPListenAgent
int TCPListenAgent::init(const SocketAddress &addr)
{
    m_Addr = addr;
    //create sockfd and
    //if(m_TCPListenSocket.init() == FAILED)
    //	return FAILED;
    if(m_TCPListenSocket.generateSocket()<0
            || m_TCPListenSocket.disableLinger()<0
            || m_TCPListenSocket.disableNagle()<0
            || m_TCPListenSocket.enableReuseaddr()<0
            || m_TCPListenSocket.setNonblock()<0
            || m_TCPListenSocket.bindAddr(m_Addr)<0
            || m_TCPListenSocket.listenOn(LISTEN_NUM)<0)
    {
        LOG_ERROR("TCPListenAgent::init()");
        return FAILED;
    }
    //Singleton<Epoll>::getInstancePtr()->registerRevent(m_iFD, this);
    //register the read I/O event
    m_epollEvent.setFd(m_TCPListenSocket.getSockFd());
    m_epollEvent.setHandler(this);
    m_epollEvent.registerREvent();
    return SUCCESSFUL;
}

//when the read event arrive, accept a connecton,
//create a ConcreteAgent and initialize it
int TCPListenAgent::recvData(void)
{
    SocketAddress oppositeAddr;
    int connfd;
    connfd = m_TCPListenSocket.acceptConnection(oppositeAddr);
    if(connfd < 0)
    {
        if(!((errno == EINTR) || (errno == EWOULDBLOCK)) )
        {
            LOG_SYSCALL("TCPListenAgent listen");
            return FAILED;
        }
    }
    TCPSocket connSock(connfd);
    if(connSock.setNonblock()<0
            || connSock.disableLinger()<0
            || connSock.disableNagle()<0)
    {
        LOG_ERROR("TCPListenAgent::recvData");
        connSock.closeSocket();
        return FAILED;
    }
    //resigerREvent and initialize the Agent's member
    TCPAgent* agent = (AgentManager::instance()).createAgent(connSock,oppositeAddr,m_epoll);
    
    if(NULL == agent)
    {
        LOG_ERROR("Agent manager create agent error !");
        connSock.closeSocket();
        return SUCCESSFUL;
    }

    if(agent->init(&(this->op)) != SUCCESSFUL)
    {
        LOG_ERROR("init agent operations error !");
        (AgentManager::instance()).recyclerAgent(agent);
        return SUCCESSFUL;
    }

	agent->connectAfter(true);
    return SUCCESSFUL;
}

//when the write event arrive, do nothing
int TCPListenAgent::sendData(void)
{
    return FAILED;
}
//extern TCPListenAgent<RequestAgent> *pRequestListenAgent;

int TCPListenAgent::recycler()
{
    if(m_epollEvent.unregisterRWEvents() < 0)
    {
        LOG_ERROR("tcplisten agent unregister from epoll error !");
        return SUCCESSFUL;
    }

    m_epollEvent.setFd(-1);
    m_epollEvent.setHandler(NULL);
    if(this->m_TCPListenSocket.closeSocket() < 0)
    {
        LOG_ERROR("tcplisten agent close socket error !");
        return SUCCESSFUL;
    }

    return SUCCESSFUL;
}

