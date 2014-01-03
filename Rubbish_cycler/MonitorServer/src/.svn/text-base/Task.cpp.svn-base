/*
 * =====================================================================================
 *
 *       Filename:  Task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/12 10:09:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Task.h"
#include "CSTCPAgent.h"
#include "AgentRegister.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "CS_Epoll.h"

Task::Task(uint32_t agent_index)
    :m_father_agent_index(agent_index)
{

}
Task::~Task()
{
    /*  
    CSTCPAgent *agent = AgentRegister::getInstance()->get_agent_from_index(m_father_agent_index);
    if(agent != NULL)
    {
        delete agent;
        m_father_agent_index = 0;
    }
    */
}

//执行回调函数，首先判断创建者Agent是否已经被析构了，如果已经被析构了就不能再执行回调了
//这时候唯一唯一能做的就是将自身析构，可以交给所谓的TaskManager完成，但是在
//被系统中仅仅使用简单的析构掉自己的操作，但是必须保证do_callback之后不会执行任何Task
//的操作了！如果还需要，那就只有使用第三方的管理方式，延迟回收

void Task::do_callback(bool ret)
{
    //执行回调的时候
    if(0 == m_father_agent_index)
    {
        m_task_cb(ret);
        return ;
    }
    if(AgentRegister::getInstance()->get_agent_from_index(m_father_agent_index) == NULL)
        delete this;
    else 
        m_task_cb(ret);
}

//解析回调的错误码，如果出错需要回调SCAgent,但是这里不知道创建的SCAgent的index，所以交给
//调用者析构出错的Agent
int Task::parse_callback_ret(void *req , RET_TYPE type)
{
    if(UNKOWN_ERROR == type)
    {
        LOG_ERROR("Task::SCAgent callback : Unknown error !");
        return -1;
    }
    else if(CONNECT_ERROR == type)
    {
        LOG_ERROR("Task::SCAgent callback : Connect error !");
        return -1;
    }
    else if(WRITE_ERROR == type)
    {
        LOG_ERROR("Task::SCAgent callback : Write error !");
        return -1;
    }
    else if(NULL == req)
    {
        LOG_ERROR("Task::SCAgent callback : Empty data error !");
        return -1;
    }

    return 0;
}

//根据uint32_t的地址结果创建SCAgent
SCAgent *Task::create_new_agent(uint32_t ip_int , uint16_t port)
{
    std::string ip;
    //首先转换成string类型的IP，然后使用char *类型的地址创建Agent
    if(int_to_string_ip(ip_int , ip) < 0)
    {
        LOG_ERROR("ExtentTask::int ip to string error !");
        return NULL;
    }

    SocketAddress oppo_addr;
    oppo_addr.setAddress(ip.c_str() , port);

    SCAgent *agent = AgentRegister::getInstance()->create_SC_agent(oppo_addr , CSEpoll::getInstance());
    if(NULL == agent)
    {
        LOG_ERROR("MigrationTask::create SC agent error !");
        return NULL;
    }

    return agent;
}
    
SCAgent *Task::create_new_agent(const std::string &ip_str , uint16_t port)
{
    SocketAddress oppo_addr;
    oppo_addr.setAddress(ip_str.c_str() , port);

    SCAgent *agent = AgentRegister::getInstance()->create_SC_agent(oppo_addr , CSEpoll::getInstance());
    if(NULL == agent)
    {
        LOG_ERROR("MigrationTask::create SC agent error !");
        return NULL;
    }

    return agent;
}
