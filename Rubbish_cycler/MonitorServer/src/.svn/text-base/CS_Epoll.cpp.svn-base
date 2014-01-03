/*
 * =====================================================================================
 *
 *       Filename:  CS_Epoll.cpp
 *
 *    Description:  实现自己的EPOLL，使用单例模式，统计信息 
 *
 *        Version:  1.0
 *        Created:  11/05/12 17:25:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "CSConfiguration.h"
#include "Log_warp.h"
#include "CS_Epoll.h"
#include "AgentRegister.h"

CSEpoll::CSEpoll()
    :Epoll()
{

}

CSEpoll::~CSEpoll()
{

}

int CSEpoll::doTask()
{
    //do something statistics
    
    AgentRegister::getInstance()->recycler_SC_agent();
    return 0;
}
