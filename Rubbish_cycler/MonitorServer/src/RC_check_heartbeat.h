/*
 * =====================================================================================
 *
 *       Filename:  RC_check_heartbeat.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/06/13 16:54:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#ifndef _H_RUBBISH_RECYCLER_HEARTBEAT_H_
#define _H_RUBBISH_RECYCLER_HEARTBEAT_H_

#include "TimerAgent.h"
#include <tr1/functional>

class Epoll;
class RCCheckHeartbeat : public TimerAgent
{
    public :
        RCCheckHeartbeat(Epoll * epl, int sec)
            :TimerAgent(epl , PERSIST_TIMER | CAN_REPEAT , sec , 0) 
        {
        }

        void set_callback(std::tr1::function<void (void)> hb)
        {
            m_heartbeat_callback = hb;
        }

        int expire_action()
        {
            m_heartbeat_callback();
            return 0;
        }

        //every heart_timeout expire , and do heartbeat callback 
        //evry counter times expire , do check callback ...
    private :
        unsigned int m_timeout;

        std::tr1::function<void (void)>  m_heartbeat_callback;
};

#endif
