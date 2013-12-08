#include <errno.h>
#include "Error.h"
#include "Epoll.h"
#include "TCPAgent.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "debug.h"

using namespace std;
//constructor
TCPAgent::TCPAgent(Epoll *epl):m_isActive(true)
{
    m_iLen = 0;
    m_iConnectTimes = -1;
    mEpollEvent.setEpoll(epl);
    this->m_Bufv.setAgent(this);
}

TCPAgent::TCPAgent(const SocketAddress& oppoAddr,Epoll *epl):
    m_Addr(oppoAddr),m_iLen(0),m_isActive(true)
{
    m_iConnectTimes = -1;
    mEpollEvent.setEpoll(epl);
    this->m_Bufv.setAgent(this);
}

//constructor
TCPAgent::TCPAgent(const TCPSocket& sock,const SocketAddress& oppoAddr,Epoll *epl):
    m_Socket(sock),m_Addr(oppoAddr),m_iLen(0),m_isActive(false)
{
    m_iConnectTimes = -1;
    TCPAgent::setState(CONNECTED);
    mEpollEvent.setEpoll(epl);
    this->m_Bufv.setAgent(this);
    this->mEpollEvent.setFd(m_Socket.getSockFd());
    mEpollEvent.setHandler(this);
}
int TCPAgent::init(struct Agent_operations *op)
{
    if(mEpollEvent.registerREvent()<0)
    {
        LOG_ERROR("mEpollEvent.registerREvent Error");
        return FAILED;
    }

    this->op = op;

    return SUCCESSFUL;
}
//destructor
TCPAgent::~TCPAgent()
{
    try
    {
        if(!m_bIsRecycler && (this->recycler() < 0))
        {
            LOG_ERROR("TCPAgent::~TCPAgent : recycler error");
        }
    }
    catch(...)
    {
        LOG_ERROR("catch exception in ~TCPAgent");
    }
}
int TCPAgent::getErrno()
{
    if(m_Socket.connectSocket(m_Addr)<0)
    {
	    LOG_ERROR("m_Socket.connectSocket(m_Addr) error");
    }
    return errno;
}

int TCPAgent::recycler()
{
    if(m_bIsRecycler)
    {
        return SUCCESSFUL;
    }
    m_bIsRecycler=true;
#ifdef DEBUG
    if(false == m_isActive)
    {
        string ip;
        ip.assign(m_Addr.getIP());
    }
#endif
    if(mEpollEvent.unregisterRWEvents()<0)
    {
        LOG_ERROR("mEpollEvent.unregisterRWEvents() error");
        return FAILED;
    }
    mEpollEvent.setFd(-1);
    mEpollEvent.setHandler(NULL);
    if(this->m_Socket.closeSocket()<0)
    {
        LOG_ERROR("m_Socket.closeSocket error");
        return FAILED;
    }
//  cout<<"nothing need to be recycled"<<endl;

    struct Context ctx;
    memset(&ctx , 0 , sizeof(ctx));
    ctx.agent = this;

    if(op != NULL && op->recycler_after != NULL)
        return op->recycler_after(ctx);

    return SUCCESSFUL;
}

//intialize the TCPAgent,set the nonblock socket
int
TCPAgent::connect(const SocketAddress &addr)
{
    m_iConnectTimes+=1;
    if(m_Socket.closeSocket()<0)
    {
        LOG_ERROR("m_Socket.closeSocket error");
    }
    m_Addr = addr;
    if( m_Socket.generateSocket()<0
            ||m_Socket.disableNagle()<0
            ||m_Socket.disableLinger()<0
            ||m_Socket.setNonblock()<0
            ||m_Socket.setKeepAlive()<0)
    {
        LOG_ERROR("TCPAgent::connect():set the socket option error ");
        return FAILED;
    }
    mEpollEvent.setFd(m_Socket.getSockFd());
    mEpollEvent.setHandler(this);
    if(mEpollEvent.registerRWEvents()<0)
    {
        return FAILED;
    }
    if(m_Socket.connectSocket(m_Addr)<0)
    {
        LOG_ERROR("NonBlock Connect Error");
    }
    return SUCCESSFUL;
}

int TCPAgent::sendData(void)
{
    return writeData();
}
int TCPAgent::recvData(void)
{
    return readData();
}
//write messages
int
TCPAgent::writeData(void)
{
    if(this->m_Bufv.write(m_Socket) <0)
    {
        LOG_ERROR("TCPAgent::writeData():m_Bufv.write(TCPSocket)");
        return FAILED;
    }
    if(this->m_Bufv.getLength() == 0)
    {
//		cout<<"closeWevent"<<endl;
        if(this->mEpollEvent.closeWevent()<0)
        {
            LOG_ERROR("this->mEpollEvent.closeWevent error");
            return FAILED;
        }
    }
    return SUCCESSFUL;
}

//write dynamic messages
int
TCPAgent::writeDynData(char *buf ,unsigned int len,BaseTask *task)
{
    if(this->m_Bufv.writeDynamic(buf,len,task) !=SUCCESSFUL)
    {
        LOG_ERROR("TCPAgent::writeDynData:m_Bufv.WriteDynamic()");
        return FAILED;
    }
    if(this->mEpollEvent.openWevent()<0)
    {
        LOG_ERROR("mEpollEvent.openWevent error");
        return FAILED;
    }
    return SUCCESSFUL;
}

int
TCPAgent::readData()
{
    int ret = this->m_Bufv.read(m_Socket);
    if(ret < 0)
    {
        LOG_ERROR("TCPAgent::readData():m_Bufv.read()");
        LOG_ERROR("opposit: %s" , m_Addr.convertToString().c_str()); //pxy
        return FAILED;
    }
    else if(CLOSED == ret)
    {
        LOG_INFO("Opposite : %s.%d close connection !" , m_Addr.getIP() , m_Addr.getPort());
        return CLOSED;
    }
//   cout<<"readData in TCPAgent"<<endl;
    return SUCCESSFUL;
}

int TCPAgent::connectAfter(bool bConnect)
{
    struct Context ctx;
    memset(&ctx , 0 , sizeof(ctx));
    ctx.agent = this;
    ctx.para.result = bConnect;

    if(op != NULL && op->connect_after != NULL)
        return op->connect_after(ctx);

    return SUCCESSFUL;
}

void TCPAgent::setState(int st)
{
    m_iConnect = CONNECTED;
    if( st == CONNECTED)
    {
        if(mEpollEvent.closeWevent()<0)
        {
            LOG_ERROR("mEpollEvent.closeWevent error");
        }
    }
}

void TCPAgent::readBack(InReq &req)
{
    struct Context ctx;
    memset(&ctx , 0 , sizeof(ctx));
    ctx.agent = this;
    ctx.para.req_in = &req;

    if(NULL == op || NULL == op->read_back)
    {
        LOG_ERROR("Agent %s read back function is NULL !" , op->name.c_str());
        return ;
    }

    return op->read_back(ctx);
}

void TCPAgent::writeBack(bool ret)
{
    struct Context ctx;
    memset(&ctx , 0 , sizeof(ctx));
    ctx.agent = this;
    ctx.para.result = ret;

    if(op != NULL && op->write_back != NULL)
        return op->write_back(ctx);
}

int TCPAgent::send_data(unsigned int cmd , unsigned int error , unsigned int para1 , unsigned int para2 , const std::string &data)
{
	struct MsgHeader head;
	memset(&head ,  0 , sizeof(head));
	head.cmd = cmd;
	head.length = data.size();
	head.error = error;
	head.para1 = para1;
	head.para2 = para2;

	int all = sizeof(head) + head.length;
	char *buf = new char[all];
	memcpy(buf , &head , sizeof(head));
	memcpy(buf + sizeof(head) , data.c_str() , head.length);

	if(this->writeDynData(buf , all , NULL) < 0)
	{
		LOG_ERROR("Write data to send buffer error !");
		return -1;
	}

	return 0;
}
