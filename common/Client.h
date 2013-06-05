#ifndef _H_CLIENT_H_
#define _H_CLIENT_H_

#include "BaseReq.h"
#include "SocketAddress.h"
#include <string>

class Epoll;
class TCPAgent;
class Client
{
    public : 
        Client(Epoll *);
        Client(Epoll * , const SocketAddress &addr);

        ~Client();

        int connect(const SocketAddress &addr);
        int connect();
        
        void set_operations(const struct Agent_operations &op);

    private :
        SocketAddress m_addr;
        TCPAgent *m_agent;
        struct Agent_operations m_ops; 
};

#endif
