/*
 * =====================================================================================
 *
 *       Filename:  TimerAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/12 17:19:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "TimerAgent.h"
#include "Log_warp.h"

TimerAgent::TimerAgent(Epoll *epl , uint32_t type 
        , int sec , long nsec)
    :m_epoll(epl)
{
    if(sec < 0)
        sec = DEFAULT_EXPIRE_SEC;
    if(nsec < 0)
        nsec = DEFAULT_EXPIRE_NSEC;

    //如果定时器既是只执行一次，那么无论超时多少次只执行一次
    if((type & ONCE_TIMER) && (type & CAN_REPEAT))
    {
        type &= ~CAN_REPEAT;
    }

    m_timer_type = type;

    m_expire_time.tv_sec = sec;
    m_expire_time.tv_nsec = nsec;
}

TimerAgent::~TimerAgent()
{
    m_read_event.unregisterRWEvents();
    close(m_timer_read_fd);
    m_epoll = NULL;
}

int TimerAgent::init()
{
    int ret = -1;

    ret = timerfd_create(CLOCK_REALTIME , 0);
    if(ret < 0)
    {
        LOG_SYSCALL_ERROR("TimerAgent::Create Timerfd error !");
        return -1;
    }

    m_timer_read_fd = ret;
    
    struct itimerspec spec;
    spec.it_value = m_expire_time;
    if(m_timer_type & PERSIST_TIMER)
    {
        spec.it_interval = m_expire_time;
    }
    else
    {
        spec.it_interval.tv_sec = 0;
        spec.it_interval.tv_nsec = 0;
    }

    ret = timerfd_settime(m_timer_read_fd , 0 , &spec , NULL);
    if(ret < 0)
    {
        LOG_SYSCALL_ERROR("TimerAgent::Timerfd Settimer error !");
        return -1;
    }

    m_read_event.setEpoll(m_epoll);
    m_read_event.setHandler(this);
    m_read_event.setFd(m_timer_read_fd);

    m_read_event.registerREvent();

    return 0;
}

int TimerAgent::recvData()
{
    int ret = -1;
    uint64_t exp_counter = 0;

RE_READ :
    ret = read(m_timer_read_fd , &exp_counter , sizeof(exp_counter));
    if(ret < 0)
    {
        if(EINTR == errno)
            goto RE_READ ;
    }
    else if(ret != sizeof(exp_counter))
    {
        LOG_ERROR("TimerAgent::Timerfd Read error : can not read Whole struct.");
        return -1;
    }
    
    //如果定时器是允许多次超时重复执行
    if(m_timer_type & CAN_REPEAT)
    {
        ret = expire_action();
        if(ret < 0)
        {
            LOG_ERROR("TimerAgent::Do expire action error !");
            return -1;
        }
    }
    else
    {
        for(uint32_t i = 0 ; i < exp_counter ; ++ i)
        {
            ret = exp_counter;
            if(ret < 0)
            {
                LOG_ERROR("TimerAgent::Do Expire Action Error !");
                continue ;
            }
        }
    }

    //如果是只执行一次的定时器，执行完超时之后直接返回
    if(m_timer_type & ONCE_TIMER)
    {
        m_read_event.unregisterRWEvents();
        delete this ;
        return 0;
    }
    
    return 0;
}
