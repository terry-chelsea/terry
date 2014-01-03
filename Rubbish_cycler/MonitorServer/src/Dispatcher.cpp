/*
 * =====================================================================================
 *
 *       Filename:  Dispatcher.cpp
 *
 *    Description:  实现调度器的功能，请求追加到缓冲队列上
 *
 *        Version:  1.0
 *        Created:  11/06/12 14:42:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Log_warp.h"
#include "Dispatcher.h"
#include "../common/util/util.h"

Dispatcher::Dispatcher()
    :Agent() ,
    m_thread_pool(NULL) ,
    m_read_fd(-1) ,
    m_write_fd(-1) ,
    m_open_write(false) ,
    m_epoll(NULL)
{

}

Dispatcher::~Dispatcher()
{
    if(!m_request_list.empty())
    {
        std::list<WorkItem *>::iterator it = m_request_list.begin();
        std::list<WorkItem *>::iterator it_end = m_request_list.end();

        while(it != it_end)
        {
            WorkItem *temp = *it;
            delete temp;
        }

        m_request_list.clear();
    }

    m_read_event.unregisterRWEvents();
    m_write_event.unregisterRWEvents();

    m_epoll = NULL;
    m_thread_pool = NULL;
}

int Dispatcher::init(Epoll *epl , ThreadPool *pool)
{
    if(NULL == epl || NULL == pool)
    {
        LOG_ERROR("Dispatcher::send NULL pointer to init Dispatcher !");
        return -1;
    }

    m_epoll = epl;
    m_thread_pool = pool;
    m_iConnect = CONNECTED;

    m_read_event.setEpoll(epl);
    m_write_event.setEpoll(epl);

    m_read_event.setHandler(this);
    m_write_event.setHandler(this);

    m_read_fd = pool->get_main_read_handle();
    m_write_fd = pool->get_main_write_handle();

    if(util::io::setNonblock(m_read_fd) < 0)
    {
        LOG_ERROR("Dispatcher::Set read handle nonblock error !");
        return -1;
    }
    if(util::io::setNonblock(m_write_fd) < 0)
    {
        LOG_ERROR("Dispatcher::Set write handle nonblock error !");
        return -1;
    }

    m_read_event.setFd(m_read_fd);
    m_write_event.setFd(m_write_fd);

    m_read_event.registerREvent();
    m_write_event.registerWEvent();

    //写段关闭写事件，等真正需要写的时候再打开
    m_write_event.closeWevent();
    m_open_write = false;

    return 0;
}

void Dispatcher::send_request(WorkItem *item)
{
    m_request_list.push_back(item);

    if(false == m_open_write)
    {
        while(m_write_event.openWevent() == FAILED);
        m_open_write = true;
    }
}

int Dispatcher::recvData()
{
    int ret = -1;
    WorkItem *p_item = NULL;

    while(1)
    {
        ret = read(m_read_fd , &p_item , sizeof(void *));
        if(ret < 0)
        {
            if(EINTR == errno)
                continue ;
            else if(EAGAIN == errno)
                break;
            else
            {
                LOG_SYSCALL_ERROR("Dispatcher::read data from pipe error !");
                return -1;
            }
        }
        if(ret != sizeof(void *))
        {
            LOG_ERROR("Dispatcher::can not read all request once !");
            return -1;
        }
        else
        {
            m_recv_list.push_back(p_item);
        }
    }

    deal_with_all_recv_request();
    return 0;
}

int Dispatcher::sendData()
{
    int ret = -1;
    WorkItem *p_item = NULL;

    while(!m_request_list.empty())
    {
        p_item = m_request_list.front();
        ret = write(m_write_fd , &p_item , sizeof(void *));
        if(ret < 0)
        {
            if(EINTR == errno)
                continue ;
            else if(EAGAIN == errno)
                break ;
            else
            {
                LOG_SYSCALL_ERROR("Dispatcher::Write data to thread pool error !");
                return -1;
            }
        }
        if(ret != sizeof(void *))
        {
            LOG_ERROR("Dispatcher::can not write Whole request !");
            return -1;
        }
        else
        {
            m_request_list.pop_front();
        }
    }

    if(m_request_list.empty())
    {
        while(m_write_event.closeWevent() == FAILED);
        m_open_write = false;
    }

    return 0;
}

void Dispatcher::deal_with_all_recv_request()
{
    std::list<WorkItem *>::iterator it = m_recv_list.begin();
    std::list<WorkItem *>::iterator it_end = m_recv_list.end();

    for( ; it != it_end ; ++ it)
    {
        (*it)->do_callback();
    }

    m_recv_list.clear();
}
