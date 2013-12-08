#ifndef _H_CLIENT_H_
#define _H_CLIENT_H_

#include "BaseReq.h"
#include "SocketAddress.h"
#include <string>
#include <tr1/functional>

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
        void set_callback(std::tr1::function<void (int)> cb);

    private :
        SocketAddress m_addr;
        TCPAgent *m_agent;
        struct Agent_operations m_ops;
        std::tr1::function<void (int)> m_cb;
        bool m_cb_flag;
        int m_ret ;
};

#endif
