/*
 * =====================================================================================
 *
 *       Filename:  Task_manager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/12 15:14:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Task_manager.h"
#include "Lib_tool.h"
#include "Log_warp.h"

TaskManager::TaskManager()
{
    get_mu_rule_task = NULL;
    get_su_rule_task = NULL;

    g_mu_extent_task = NULL;
    g_su_extent_task = NULL;
}

TaskManager::~TaskManager()
{
    if(get_mu_rule_task != NULL)
    {
        delete get_mu_rule_task ;
        get_mu_rule_task = NULL ;
    }
    if(get_su_rule_task != NULL)
    {
        delete get_su_rule_task ;
        get_su_rule_task = NULL ;
    }
    if(g_mu_extent_task != NULL)
    {
        delete g_mu_extent_task;
        g_mu_extent_task = NULL;
    }
    if(g_su_extent_task != NULL)
    {
        delete g_su_extent_task;
        g_su_extent_task = NULL;
    }
}

bool TaskManager::RS_is_getting_rule(RULE_TYPE type)
{
    if(MU_RULER == type)
    {
        if(NULL == get_mu_rule_task)
            return false;
        else 
            return true;
    }
    else
    {
        if(NULL == get_su_rule_task)
            return false ;
        else 
            return true;
    }
}

bool TaskManager::RS_need_continue_update_rule(RULE_TYPE type)
{
    if(MU_RULER == type)
    {
        if(get_mu_rule_request_list.empty())
            return false ;
        else
            return true ;
    }
    else 
    {
        if(get_su_rule_request_list.empty())
            return false ;
        else
            return true ;
    }
}

GetRuleTask *TaskManager::get_get_rule_task(RULE_TYPE type)
{
    if(MU_RULER == type)
        return this->get_mu_rule_task ;
    else
        return this->get_su_rule_task ;
}

GetRuleTask *TaskManager::create_get_rule_task(RULE_TYPE type , uint32_t index)
{
    if(MU_RULER == type)
    {
        if(this->get_mu_rule_task != NULL)
        {
            LOG_ERROR("TaskManager::Get mu rule exist when create new Task !");
            return NULL;
        }
    }
    if(SU_RULER == type)
    {
        if(this->get_su_rule_task != NULL)
        {
            LOG_ERROR("TaskManager::Get su rule exist when create new Task !");
            return NULL;
        }
    }

    GetRuleTask *task = new GetRuleTask(index , type);

    if(MU_RULER == type)
        this->get_mu_rule_task = task;
    else
        this->get_su_rule_task = task;

    return task;
}

void TaskManager::append_new_update_request(RULE_TYPE type , CSNoticeAgent *agent)
{
    if(MU_RULER == type)
       this->get_mu_rule_request_list.push_back(agent);
    else
       this->get_su_rule_request_list.push_back(agent);
}

CSNoticeAgent *TaskManager::delete_first_update_request(RULE_TYPE type)
{
    CSNoticeAgent *agent = NULL;
    if(MU_RULER == type)
    {
        if(get_mu_rule_request_list.empty())
            return NULL ;
        else
        {
            agent = get_mu_rule_request_list.front();
            get_mu_rule_request_list.pop_front();
        }
    }
    else
    {
        if(get_su_rule_request_list.empty())
            return NULL ;
        else
        {
            agent = get_su_rule_request_list.front();
            get_su_rule_request_list.pop_front();
        }
    }

    return agent ;
}

void TaskManager::delete_get_rule_task(RULE_TYPE type)
{
    if(MU_RULER == type)
    {
        if(this->get_mu_rule_task != NULL)
            delete this->get_mu_rule_task;

        this->get_mu_rule_task = NULL;
    }
    else
    {
        if(this->get_su_rule_task != NULL)
            delete this->get_su_rule_task;

        this->get_su_rule_task = NULL;
    }
}

//判断是否可以执行桶迁移操作，可以执行的条件是：
//1、当前系统不处于全局的桶扩展状态,如果当前系统没有扩展Task说明不处于扩展
//2、如果处于扩展阶段，但是可能一部分节点已经完成了扩展，而另外节点没有完成
//在完成节点上执行迁移操作是允许的
bool TaskManager::can_exec_migration(RULE_TYPE type , uint32_t src , uint32_t dest)
{
    if(MU_RULER == type)
    {
        if(NULL == this->g_mu_extent_task)
            return true ;
        else 
            return this->g_mu_extent_task->can_exec_migration(src , dest);
    }
    else
    {
        if(NULL == this->g_su_extent_task)
            return true ;
        else
            return this->g_su_extent_task->can_exec_migration(src , dest);
    }
}

//判断是否处于桶扩展操作，可移植性的原则是：
//1、当前系统不处于扩展状态，也就是任何一个节点的扩展操作都完成了。
//2、当前系统没有任何一个桶扩展操作在执行，暂时没实现，需要查看数据库完成
bool TaskManager::can_exec_extent(RULE_TYPE type)
{
    if(MU_RULER == type && NULL == this->g_mu_extent_task)
        return true ;
    
    if(SU_RULER == type && NULL == this->g_su_extent_task)
        return true ;

    return false;
}

ExtentTask *TaskManager::create_extent_task(RULE_TYPE type , uint32_t index , Extent_Info info)
{
    if(MU_RULER == type )
    {
         if(this->g_mu_extent_task != NULL)
         {
             LOG_ERROR("TaskManager::MU Extent task exist when create new extent task");
             return NULL;
         }
    }
    else 
    {
         if(this->g_su_extent_task != NULL)
         {
             LOG_ERROR("TaskManager::SU Extent task exist when create new extent task");
             return NULL;
         }
    }

    ExtentTask *task = new ExtentTask(index , type , info);

    if(MU_RULER == type)
        this->g_mu_extent_task = task;
    else 
        this->g_su_extent_task = task;

    return task;
}

void TaskManager::delete_extent_task(RULE_TYPE type)
{
    if(MU_RULER == type)
    {
        if(this->g_mu_extent_task != NULL)
            delete this->g_mu_extent_task;
        this->g_mu_extent_task = NULL;
    }
    else 
    {
        if(this->g_su_extent_task != NULL)
            delete this->g_su_extent_task;
        this->g_su_extent_task = NULL;
    }
}
