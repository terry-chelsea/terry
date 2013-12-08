#include <time.h>
#include <string.h>
#include <errno.h>
#include "Epoll.h"
#include "EpollEvent.h"
#include "TCPAgent.h"
#include "Error.h"
#include "AgentManager.h"
#include "debug.h"

#define EPOLL_TIMEOUT_LEN 5000
Epoll::Epoll(void):
    mEpollEvents(NULL),
    mEpollFd(-1)
{
//    gettimeofday(&mCurrent, NULL);
}

Epoll::~Epoll(void)
{
    if( mEpollEvents !=NULL )
        delete [] mEpollEvents;
}

int Epoll::getEpollFd()const
{
    return mEpollFd;
}

int Epoll::initialize( unsigned int fdsize)
{
    mEventSize = fdsize;
    mEpollFd = epoll_create( mEventSize );
    if( mEpollFd < 0)
    {
        LOG_ERROR("Epoll:initialize");
        return -1;
    }
    mEpollEvents = new epoll_event[fdsize];
    memset( mEpollEvents, 0, sizeof(epoll_event)*fdsize);
    return 0;
}

const struct timeval& Epoll::getCurrent(void)
{
    return mCurrent;
}

int Epoll::doEvent( EpollEvent* ptr,
                    int fd,
                    int op,
                    unsigned int events)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof( struct epoll_event));
    ev.events = events;
    ev.data.ptr = ptr;
    if( epoll_ctl( mEpollFd, op, fd, &ev) < 0)
    {
        LOG_ERROR("Epoll:doEvent");
        return FAILED;
    }
    return SUCCESSFUL;
}


void Epoll::run(void)
{

    int nfds = 0;

    EpollEvent* event = NULL;

    for(;;)
    {

        if( (nfds = epoll_wait( mEpollFd, mEpollEvents,	mEventSize, EPOLL_TIMEOUT_LEN )) <0 )
        {
//			printf("epollfd: %d, mEventsize: %d, timeout:%d\n",mEpollFd, mEventSize, EPOLL_TIMEOUT_LEN);
            if( errno == EINTR )
            {
                continue;
            }
            else
            {
                LOG_ERROR("Epoll:epoll_wait");
            }
        }

        if( gettimeofday( &mCurrent, NULL) < 0 )
            LOG_ERROR("Epoll:gettimeofday");

//		unsigned long long t = 100000 * mCurrent.tv_sec + mCurrent.tv_usec;
        //	printf("epoll: time: %d\n",t/1000);

        for( int i= 0; i< nfds; i++)
        {
            event = (EpollEvent*)mEpollEvents[i].data.ptr;
            if ( event == NULL )
                continue;
            Agent *agent=event->getHandler();
            if( NULL == agent)
            {
                LOG_ERROR("Epoll::agent == NULL");
                continue;
            }
            if ( mEpollEvents[i].events & EPOLLERR
                    || mEpollEvents[i].events & EPOLLHUP )
            {
                if(agent->getState() == CONNECTING)//for the connect
                {
                    if ( agent->getErrno() == EISCONN)
                    {
                        agent->setState(CONNECTED);
                        if(agent->connectAfter(true) < 0)
                        {
                            (AgentManager::instance()).recyclerAgent(agent);
                            continue;
                        }
                        agent->resetConnect();
                        continue;
                    }
                    else
                    {
                        if(agent->allowReconnect())
                        {
                            SocketAddress addr;
                            if(SUCCESSFUL == (agent->getOppAddr(addr)))
                            {
                                if( agent->connect(addr) < 0)
                                {
                                }
                            }
                            else
                            {
                                LOG_ERROR("OppAddr error");
                                (AgentManager::instance()).recyclerAgent(agent);
                            }
                            continue;
                        }
                        else
                        {
                            agent->resetConnect();
                            if(agent->connectAfter(false)<0)
                            {
                                (AgentManager::instance()).recyclerAgent(agent);
                                continue;
                            }
                            else
                                continue;
                        }
                    }
                }
                else
                {
                    int ret = agent->recvData();
                    if(ret != SUCCESSFUL)
                    {
                        (AgentManager::instance()).recyclerAgent(agent);
                        continue;
                    }
                }
            }
            if(mEpollEvents[i].events & EPOLLOUT )
            {
                if(CONNECTED == (agent->getState()))
                {
                    if( agent->sendData() < 0)
                    {
                        (AgentManager::instance()).recyclerAgent(agent);
                        continue;
                    }
                }
                else
                {
                    agent->setState(CONNECTED);
                    if(agent->connectAfter(true) < 0)
                    {
                        (AgentManager::instance()).recyclerAgent(agent);
                    }
                    continue;
                }
            }
            if( mEpollEvents[i].events & EPOLLIN)
            {
                if(agent->recvData() != SUCCESSFUL)
                {
                    (AgentManager::instance()).recyclerAgent(agent);
                    continue;
                }
            }
        }
        if(this->doTask() < 0)
        {
            LOG_ERROR("Deal With Task Error");
        }
    }
}

int Epoll::doTask()
{
    int ret = AgentManager::getInstancePtr()->clean_agent();
    if(ret > 0)
        LOG_INFO("Recycler %d agent in epoll cycler..." , ret);
    return 0;
}
