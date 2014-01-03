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

#include "WorkItem.h"
#include "ThreadPool.h"
#include "Log_warp.h"

int WorkItem::process_after()
{
    int ret =  ThreadPool::getInstance()->post_request(this);
    if(ret < 0)
    {
        LOG_ERROR("WorkItem::process after error !");
        return -1;
    }
    return 0;
}
