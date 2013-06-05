#include "BufferManager.h"
//#include "TCPSocket.h"
#include "TCPAgent.h"
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include "Error.h"
#include "debug.h"

using namespace std;

BufferManager::BufferManager()
    : mLastIov(NULL),
      m_Agent(NULL),
      m_iOffset(0),
      m_iLen(0),
      m_bReadHead(true),
      m_bInit(true)
{
}

BufferManager::~BufferManager()
{
    try
    {
        m_Agent = NULL;
        list<iov_req>::iterator aIt = mIovList.begin();
        while(  aIt != mIovList.end() )
        {
            if( (*aIt).mComplete )
            {
                if ( (*aIt).mIov.iov_base )
                {
                    delete []( char*)(*aIt).mIov.iov_base;
                    (*aIt).mIov.iov_base = NULL;
                }
            }
            ++aIt;
        }
        mIovList.clear();
        if( mLastIov != NULL)
        {
            delete [](char*)mLastIov;
            mLastIov=NULL;
        }
    }
    catch(...)
    {
        LOG_ERROR("catch exception in ~BufferManager");
    }
}
int BufferManager::clear(void)
{
//	cout<<"the clear is called"<<endl;
    list<iov_req>::iterator aIt = mIovList.begin();
    while(  aIt != mIovList.end() )
    {
        if( (*aIt).mComplete )
            if ( (*aIt).mIov.iov_base )
            {
                delete []( char*)(*aIt).mIov.iov_base;
                (*aIt).mIov.iov_base = NULL;
            }
        ++aIt;
    }
    mIovList.clear();
    if( mLastIov != NULL)
    {
        delete [](char*)mLastIov;
        mLastIov = NULL;
    }
    return 0;
}
void BufferManager::setAgent(Agent *agent)
{
    this->m_Agent=agent;
}

int BufferManager::writeDynamic(char *buffer, unsigned int len , BaseTask *task = NULL)
{
        
    struct iov_req iov(buffer, len, task);
    mIovList.push_back(iov);
    return SUCCESSFUL;
}

int BufferManager::writeDynamic
(
    char *buffer,
    unsigned int len,
    SocketAddress &addr,
    BaseTask *task = NULL
)
{
    return 0;
}

int BufferManager::write( TCPSocket& sock)
{
    const unsigned int reqlen = (unsigned int)mIovList.size();
    struct iovec outdata[ reqlen ];
    memset(outdata, 0, reqlen*sizeof(struct iovec));
    list<iov_req>::iterator aIt , pIt;
    aIt = mIovList.begin();
    unsigned int num = 0;

    while( aIt != mIovList.end() )
    {
        outdata[num].iov_base = (*aIt).mIov.iov_base;
        outdata[num].iov_len = (*aIt).mIov.iov_len;
        ++num;
        ++aIt;
    }
    int ret = -1;
    while( ( ret = sock.writevSocket( outdata, reqlen) )< 0 )
    {
        if( errno == EINTR )
            continue;
        if( errno != EWOULDBLOCK )
        {
            handleWriteError();
            return FAILED;
        }
    }
    int download = ret;
    aIt = mIovList.begin();
    while( ret > 0 && aIt != mIovList.end() )
    {
        pIt = aIt++;
        if((unsigned int)ret >= (*pIt).mIov.iov_len )
        {
            ret -= (*pIt).mIov.iov_len;
            if( (*pIt).mComplete )
            {
                if ( (*pIt).mIov.iov_base )
                {
                    delete []( char*)(*pIt).mIov.iov_base;
                    (*pIt).mIov.iov_base = NULL;
                }
            }
            else
            {
                if( mLastIov != NULL)
                {
                    delete [](char*)mLastIov;
                    mLastIov = NULL;
                }
            }
            if((*pIt).mTask != NULL)
            {
                (*pIt).mTask->writeBack(true);
            }
            else
            {
                if(m_Agent != NULL)
                {
                    m_Agent->writeBack(true);
                }
                else
                {
                    LOG_ERROR("the BufferManager don't relate a Agent");
                    return FAILED;
                }
            }
            mIovList.erase(pIt);
        }
        else
        {
            if((*pIt).mComplete )
            {
                mLastIov = (*pIt).mIov.iov_base;
                (*pIt).mComplete = false;
            }
            (*pIt).mIov.iov_base = (void*)((char*)( (*pIt).mIov.iov_base) +ret);
            (*pIt).mIov.iov_len -= (unsigned int)ret;
            break;
        }
    }
    return download;
}


unsigned int BufferManager::getLength(void )const
{
    return (unsigned int)mIovList.size();

}

int BufferManager::read(TCPSocket &sock)
{
    int ret;
    if(m_bInit)
    {
        m_InReq.ioBuf=NULL;
        memset(&m_InReq.m_msgHeader,0,HEADER_SIZE);
        m_bInit=false;
        m_iLen =HEADER_SIZE;
    }
    if(m_bReadHead)
    {
        if((ret=sock.readSocket(((char*)(&(m_InReq.m_msgHeader)))+m_iOffset,\
                                HEADER_SIZE-m_iOffset))<0)
        {
            if(!((errno == EWOULDBLOCK) || (errno == EINTR)))
            {
                LOG_SYSCALL("BufferManager::read()");
                return FAILED;
            }
            return SUCCESSFUL;
        }
        else if(ret == 0)
        {
            LOG_ERROR("the opposit close the connection");
            return FAILED;
        }
        m_iOffset += (unsigned int)ret;
        if(m_iOffset == HEADER_SIZE)
        {
            m_iOffset = 0;
            m_iLen = m_InReq.m_msgHeader.length;
            if(m_iLen>0)
            {
                m_InReq.ioBuf=new char[m_iLen+1];
                m_bReadHead = false;
            }
            else if(m_iLen == 0)
            {
                if(m_Agent!=NULL)
                {
                    m_Agent->readBack(m_InReq);
                    this->m_bInit=true;
                }
                this->m_bReadHead=true;
                return SUCCESSFUL;
            }
            else
            {
                m_bReadHead=true;
                LOG_ERROR("the data length is not illegal");
                return FAILED;
            }
        }
    }
    if(!m_bReadHead)
    {
        if((ret=sock.readSocket(m_InReq.ioBuf+m_iOffset,\
                                m_iLen-m_iOffset))<0)
        {
            if(!((errno == EWOULDBLOCK) || (errno == EINTR)))
            {
                LOG_ERROR("readSocket in BufferMananger");
                return FAILED;
            }
            return SUCCESSFUL;
        }
        else if(ret == 0)
        {
            LOG_ERROR("the opposit close the connection in readData");
            return FAILED;
        }
        m_iOffset+=(unsigned int)ret;

        if(m_iOffset == m_iLen)
        {
            m_InReq.ioBuf[m_iLen]=0;
            m_iOffset = 0;
            m_iLen=0;
            this->m_bReadHead=true;
            if(m_Agent != NULL)
            {
                m_Agent->readBack(m_InReq);
                this->m_bInit=true;
            }
            if(m_InReq.ioBuf != NULL)
            {
                delete [] m_InReq.ioBuf;
                m_InReq.ioBuf=NULL;
            }
            return SUCCESSFUL;
        }
    }
    return (int)(m_iLen-m_iOffset);
}

void BufferManager::handleWriteError()
{
    list<iov_req>::iterator aIt = mIovList.begin();
    while(aIt != mIovList.end())
    {
        if((*aIt).mTask != NULL)
        {
            (*aIt).mTask->writeBack(false);
        }
        else
        {
            if(m_Agent != NULL)
            {
                m_Agent->writeBack(false);
            }
            else
            {
                LOG_ERROR("the BufferManager don't relate a Agent");
            }
        }
        ++aIt;
    }
}
