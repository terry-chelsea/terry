/*
 * =====================================================================================
 *
 *       Filename:  Register_agent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/23/13 17:20:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_REGISTER_NODE_AGENT_H_
#define _H_REGISTER_NODE_AGENT_H_

#include "CSTCPAgent.h"
#include "RulerManager.h"

class RegisteNodeAgent : public CSTCPAgent
{
    public : 
        RegisteNodeAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);

        ~RegisteNodeAgent()
        {
        }

        void readBack(InReq &req);

        int recycler();

    private :
        RULE_TYPE m_type;
};
#endif
