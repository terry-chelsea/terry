/*
 * =====================================================================================
 *
 *       Filename:  Init_rule.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 10:19:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_INIT_RULE_H_
#define _H_INIT_RULE_H_

#include <iostream>
#include <cstring>
#include <string>
#include <tr1/functional>

#include "SCAgent.h"
#include "RulerManager.h"

class RuleInit
{
    public :
        RuleInit(bool cs , bool rs);
        ~RuleInit();

        int init_rule(std::tr1::function<void (bool)> ret_callback);

    private :
        void do_call_back(bool ret)
        {
            m_ret_cb(ret);
        }

        int get_whole_rule_from_db();

        int get_whole_rule_from_cs();

        int get_rule_from_cs(RULE_TYPE type);
        
        void done_get_rule_from_cs(void *req , RET_TYPE type , void *data);

        int  get_rule_ack_from_cs(RULE_TYPE type , InReq *req);
    private :
        bool     m_is_cs;
        bool     m_is_rs;
        uint32_t m_mu_rule_agent_index;
        uint32_t m_su_rule_agent_index;
        std::tr1::function<void (bool)> m_ret_cb;
};


#endif
