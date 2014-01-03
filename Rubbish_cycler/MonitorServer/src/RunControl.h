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
#include <list>

#include "SCAgent.h"
#include "RulerManager.h"
#include "Init_rule.h"
#include "../common/sys/Singleton.h"

struct Register_Info
{
    uint32_t index;
    uint32_t ip;
};

class InitSystemTask;
class RunControl : public Singleton<RunControl>
{
    public : 
        RunControl();
        ~RunControl();

        int prepare_run();
        int run();

        bool can_register(RULE_TYPE);
        void node_register(struct Register_Info , RULE_TYPE);
        void register_node_recycler(uint32_t index , RULE_TYPE type);
        
    private :
        int start_timer();

        int start_server();

        int start_rs();
        int start_cs();
        int start_cs_and_rs();

        int listen_system_client();
        int listen_cs_agent();

        int wait_node_initialize();

        void finish_register(RULE_TYPE);
        void notice_all_node(RULE_TYPE);
        void after_init_system(bool , RULE_TYPE);

    private :
        bool m_running;             //CS是否已经启动
        bool m_su_finish;           //SU节点是否完成了注册
        bool m_mu_finish;           //MU节点是否完成了注册

        //所有注册节点注册的信息
        std::list<struct Register_Info> m_su_register_list;
        std::list<struct Register_Info> m_mu_register_list;

        InitSystemTask *m_init_task;
};

#endif
