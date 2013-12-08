/*
 * =====================================================================================
 *
 *       Filename:  Server.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/13 21:39:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Server.h"
#include "TCPListenAgent.h"
#include "TCPAgent.h"
#include "Epoll.h"
#include "debug.h"
#include "SocketAddress.h"

Server::Server(Epoll *epl)
    :listen_agent(epl , NULL) ,
    m_is_running(false)
{
}

Server::Server(Epoll *epl , struct Agent_operations op)
    :listen_agent(epl , &op) ,
    m_is_running(false)
{
}

void Server::set_read_func(Void_op op)
{
    this->listen_agent.set_read_op(op);
}

void Server::set_write_func(Void_op op)
{
    this->listen_agent.set_write_op(op);
}

void Server::set_connect_func(Int_op op)
{
    this->listen_agent.set_connect_op(op);
}

void Server::set_recycler_func(Int_op op)
{
    this->listen_agent.set_recycler_op(op);
}

void Server::set_agent_name(string name)
{
    this->listen_agent.set_agent_name(name);
}

string Server::get_agent_name()
{
    return this->listen_agent.get_agent_name();
}

int Server::init(const SocketAddress &addr)
{
    if(listen_agent.init(addr) < 0)
    {
        LOG_ERROR("Init tcplisten agent error : ip is %s and port : %d" , 
                addr.getIP() , addr.getPort());
        return -1;
    }

    m_is_running = true;
    return 0;
}

int Server::stop()
{
    if(!m_is_running)
    {
        LOG_WARNING("Server is not init !");
        return SUCCESSFUL;
    }

    listen_agent.recycler();

    return SUCCESSFUL;
}
