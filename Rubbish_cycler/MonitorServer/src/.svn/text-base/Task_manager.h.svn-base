/*
 * =====================================================================================
 *
 *       Filename:  Task_manager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/12 15:10:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TASK_MANAGER_H_
#define _H_TASK_MANAGER_H_
//管理那些需要特殊标记的Task

#include <iostream>
#include <cstring>
#include <list>

#include "CSNoticeAgent.h"
#include "RulerManager.h"
#include "Get_rule_task.h"
#include "Extent_task.h"
#include "../common/sys/Singleton.h"

class TaskManager : public Singleton<TaskManager>
{
    friend class Singleton<TaskManager>;

    public : 
        TaskManager();
        ~TaskManager();

        //判断RS是否正在拉取规则，在收到任务的时候判断
        bool RS_is_getting_rule(RULE_TYPE type);

        //判断RS是否还有其他拉取规则请求，在完成一次规则的拉取的时候判断
        bool RS_need_continue_update_rule(RULE_TYPE);

        GetRuleTask *get_get_rule_task(RULE_TYPE);

        //创建一个拉取规则的Task
        GetRuleTask *create_get_rule_task(RULE_TYPE , uint32_t);

        //添加一个新的请求，在正在拉取规则的时候如果有新的请求，添加
        void append_new_update_request(RULE_TYPE type , CSNoticeAgent *agent);

        CSNoticeAgent *delete_first_update_request(RULE_TYPE type);

        //完成之后删除一个Task
        void delete_get_rule_task(RULE_TYPE type);

        //判断是否能够执行迁移操作
        bool can_exec_migration(RULE_TYPE , uint32_t src , uint32_t dest);

        //判断是否能够执行桶扩展操作
        bool can_exec_extent(RULE_TYPE type);

        //创建一个新的桶扩展Task
        ExtentTask *create_extent_task(RULE_TYPE , uint32_t , Extent_Info );

        //析构一个桶扩展操作的Task
        void delete_extent_task(RULE_TYPE);

        ExtentTask *get_extent_task(RULE_TYPE type)
        {
            if(MU_RULER == type)
                return g_mu_extent_task;
            else if(SU_RULER == type)
                return g_su_extent_task;
            else
                return NULL;
        }

    private :
        //对于RS，是否有正在更新MU规则的Task
        GetRuleTask *get_mu_rule_task;
        //对于RS，是否有正在更新SU规则的Task
        GetRuleTask *get_su_rule_task;

        //对于RS，当前又正在更新规则的操作，但是仍有等待更新操作的计数
        std::list<CSNoticeAgent *> get_mu_rule_request_list;
        std::list<CSNoticeAgent *> get_su_rule_request_list;

        //对于CS，是有有MU桶扩展的Task，用来判断是否可以相应MPC的请求
        ExtentTask *g_mu_extent_task;
        ExtentTask *g_su_extent_task;

};

#endif
