/*
 * =====================================================================================
 *
 *       Filename:  TimerAgent.h
 *
 *    Description:  使用timerfd实现定时器的功能
 *
 *        Version:  1.0
 *        Created:  11/06/12 17:03:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TIMER_AGENT_H_
#define _H_TIMER_AGENT_H_

#include <iostream>
#include <cstring>
#include <sys/timerfd.h>
#include <sys/time.h>
#include <errno.h>

#include "../common/comm/Agent.h"
#include "../common/comm/Epoll.h"
#include "../common/comm/EpollEvent.h"

#define DEFAULT_EXPIRE_NSEC 0
#define DEFAULT_EXPIRE_SEC  1

    //持久化的定时器
const uint32_t PERSIST_TIMER = 0x1 ;
    //只执行一次的定时器，超时之后自己析构
const uint32_t ONCE_TIMER = 0x2 ; 
    //如果多次超时，只执行一次超时函数
const uint32_t CAN_REPEAT = 0X4 ;

class TimerAgent : public Agent
{
    public :
        TimerAgent(Epoll *epl , uint32_t type = PERSIST_TIMER | CAN_REPEAT , 
                int sec = DEFAULT_EXPIRE_SEC 
                , long nsec = DEFAULT_EXPIRE_NSEC);

        virtual ~TimerAgent();

        int init();
        //定时器超时会触发读事件
        virtual int recvData();
        virtual int sendData()
        {
            return 0;
        }

        virtual int expire_action()
        {
            return 0;
        }

    protected :
        struct timespec m_expire_time;
        uint32_t m_timer_type;
        
        //epoll
        Epoll *m_epoll;
        int m_timer_read_fd;
        EpollEvent m_read_event;
};


#endif
