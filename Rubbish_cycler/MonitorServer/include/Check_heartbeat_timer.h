/*
 * =====================================================================================
 *
 *       Filename:  Check_heartbeat_timer.h
 *
 *    Description:  查看心跳信息的定时器，判断心跳是否已经超时
 *
 *        Version:  1.0
 *        Created:  11/06/12 17:52:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_CHECK_HEARTBEAT_TIMER_H_
#define _H_CHECK_HEARTBEAT_TIMER_H_

#include <iostream>
#include <list>
#include <cstring>

#include "TimerAgent.h"

#define DEFAULT_TINEOUT  60

class CheckHeartbeatAgent : public TimerAgent
{
    public :
        CheckHeartbeatAgent(Epoll *epl , int sec , int nsec , int timeout);
        CheckHeartbeatAgent(Epoll *epl , int timeout);
        ~CheckHeartbeatAgent();

        int expire_action();
        int add_heartbeat_agent();

    private :
//        std::list<HeartbeatAgent *> m_heartbeat_agent_list;
        int  m_heartbeat_timeout;
};

#endif
