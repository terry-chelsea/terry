/*
 * =====================================================================================
 *
 *       Filename:  Report_MPC_Timer.h
 *
 *    Description:  向MPC定时发送最新的系统负载的定时器
 *
 *        Version:  1.0
 *        Created:  11/06/12 18:11:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_REPORT_MPC_TIMER_H_
#define _H_REPORT_MPC_TIMER_H_

#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include "TimerAgent.h"
#include "SCAgent.h"

class ReportMPCTimer : public TimerAgent
{
    public :
        ReportMPCTimer(Epoll * epl , int sec , int nsec);
        ReportMPCTimer(Epoll *epl);
        ~ReportMPCTimer();

        int expire_action();
    private :
        int send_data_and_callback(MsgHeader &header , const std::string &out);

        void done_send_message_to_mpc(void *req , RET_TYPE ret , void *data);

    private :
       uint32_t m_report_agent_index; 
};

#endif
