/*
 * =====================================================================================
 *
 *       Filename:  Server.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/21/13 18:10:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_SERVER_H_
#define _H_SERVER_H_

#include "TCPListenAgent.h"
#include "BaseReq.h"
#include <string>

class TCPListenAgent;
class TCPAgent;
class Epoll;
class SocketAddress;

class Server
{
    public : 
        Server(Epoll *);
        Server(Epoll * , struct Agent_operations );

        int init(const SocketAddress &);
        int stop();

        void set_read_func(Void_op );
        void set_write_func(Void_op );
        void set_connect_func(Int_op );
        void set_recycler_func(Int_op );
        void set_agent_name(std::string);
		std::string get_agent_name();

    private :
        void copy_operation(struct Agent_operations);

    private :
        TCPListenAgent listen_agent;
        bool  m_is_running;
};

#endif
