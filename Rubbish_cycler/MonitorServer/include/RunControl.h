/*
 * =====================================================================================
 *
 *       Filename:  RunControl.h
 *
 *    Description:  CS系统初始化
 *
 *        Version:  1.0
 *        Created:  11/05/12 17:09:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_RUN_CONTROL_H_
#define _H_RUN_CONTROL_H_

#include<iostream>
#include<cstring>
#include<string>
#include<vector>

#include "SCAgent.h"
#include "RulerManager.h"
#include "Init_rule.h"

class RunControl
{
    public : 
        RunControl();
        ~RunControl();

        int prepare_run();
        int run();

    private :
        int start_timer();

        void start_server(bool);

        int start_rs();
        int start_cs();
        int start_cs_and_rs();

        int listen_system_client();
        int listen_cs_agent();
        
    private :
        bool m_init_ret;       //是否初始化成功，一般使用不到
        RuleInit *m_init_rule;
};

#endif
