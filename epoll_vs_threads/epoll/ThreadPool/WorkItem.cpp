/*
 * =====================================================================================
 *
 *       Filename:  WorkItem.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 09:41:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "ThreadPool.h"
#include "../common/debug.h"

WorkItem::WorkItem() : 
    m_exec_result(false) , 
    m_process(NULL) , 
    m_para(NULL) ,
    exec_after(false) 
{

}

WorkItem::~WorkItem()
{
    if(!m_exec_result)
    {
        this->process();
        this->do_callback();
    }
}

int WorkItem::process_after()
{
    int ret =  ThreadPool::getInstancePtr()->post_request(this);
    if(ret < 0)
    {
        LOG_ERROR("WorkItem::process after error !");
        return -1;
    }
    return 0;
}

int WorkItem::pre_process()
{
    LOG_INFO("Before execurate a work item...");
    return 0;
}

void WorkItem::process()
{
    m_exec_result = m_process(this);
    exec_after = true;
}

void *WorkItem::get_exec_result()
{
    return m_para;
}

void WorkItem::set_exec_result(void *ret)
{
    m_para = ret;
}
