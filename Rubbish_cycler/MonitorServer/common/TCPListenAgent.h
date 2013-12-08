#ifndef _TCPLISTENAGENT_H
#define _TCPLISTENAGENT_H

#include "TCPSocket.h"
#include "EpollEvent.h"
#include "Epoll.h"
#include "SocketAddress.h"
#include "BaseReq.h"
#include "Agent.h"
#define LISTEN_NUM  50000

using namespace std;
class TCPListenAgent : public Agent
{
public:
    TCPListenAgent(Epoll* epl , struct Agent_operations *ops)
    {
        m_epollEvent.setEpoll(epl);
        m_epoll = epl;
        this->op = *ops;
    }
    ~TCPListenAgent() {}
    virtual int init(const SocketAddress &);
    virtual int recycler();
    virtual int recvData(void);
    virtual int sendData(void);

public : 
    void set_read_op(Void_op op)
    {
        this->op.read_back = op;
    }

    void set_write_op(Void_op op)
    {
        this->op.write_back = op;
    }

    void set_connect_op(Int_op op)
    {
        this->op.connect_after = op;
    }

    void set_recycler_op(Int_op op)
    {
        this->op.recycler_after = op;
    }

    void set_agent_name(string name)
    {
        this->op.name = name;
    }

    string get_agent_name()
    {
        return this->op.name;
    }

private:
    TCPSocket m_TCPListenSocket;
    SocketAddress m_Addr;
//	int m_iPort;
    EpollEvent m_epollEvent;
    Epoll *m_epoll;
    
    struct Agent_operations op;
};

#endif
