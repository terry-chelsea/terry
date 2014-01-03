/*
 * =====================================================================================
 *
 *       Filename:  CSNoticeAgent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 22:08:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_CS_NOTICE_AGENT_H_
#define _H_CS_NOTICE_AGENT_H_

#include "CSTCPAgent.h"
#include "RulerManager.h"

//在RS中，CS通知RS更新规则的时候会创建出这种Agent
class CSNoticeAgent : public CSTCPAgent
{
    public :
        CSNoticeAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);
        ~CSNoticeAgent();

        int init();

        void readBack(InReq &req);

        int get_new_rule();

        void done_get_new_rule(bool ret);

    private :
        RULE_TYPE m_rule_type;
};

#endif
