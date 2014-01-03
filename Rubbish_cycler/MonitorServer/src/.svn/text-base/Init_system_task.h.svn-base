/*
 * =====================================================================================
 *
 *       Filename:  Init_system_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/08/13 11:52:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_INIT_SYSTEM_TASK_H_
#define _H_INIT_SYSTEM_TASK_H_

#include <iostream>
#include <string>
#include <vector>
#include <tr1/functional>
#include "Task.h"
#include "RulerManager.h"

typedef struct init_info
{
    std::vector<uint32_t> all_ip_list;
    int dup;
    int mod;
}Init_info;

class WorkItem;
class InitSystemTask : public Task
{
    public : 
        InitSystemTask(uint32_t , RULE_TYPE , Init_info);
        ~InitSystemTask();

        int init(std::tr1::function<void (bool , RULE_TYPE)>);

    private :
        void layout_all_bucket(std::vector<struct HashRule> &);

        void after_init_db_rule(bool );

        void do_callback(bool);

        int init_memory_rule();

    private : 
        uint32_t     m_agent_index;
        Init_info    m_info;
        RULE_TYPE    m_type;
        WorkItem    *m_item;

        std::tr1::function<void (bool , RULE_TYPE)>  m_callback_func;
};

#endif
