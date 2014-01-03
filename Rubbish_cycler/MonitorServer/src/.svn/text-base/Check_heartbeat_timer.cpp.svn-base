/*
 * =====================================================================================
 *
 *       Filename:  Check_heartbeat_timer.cpp
 *
 *    Description:  定时的查看心跳信息是否已经超时
 *
 *        Version:  1.0
 *        Created:  11/06/12 18:02:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Check_heartbeat_timer.h"
#include "HeartbeatAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "HB_manager.h"

#include <stdlib.h>

/*  
void test_heartbeat()
{
    std::cerr<<"test_heartbeat"<<std::endl;
    srand(time(NULL));
    const char * mu_ips[] = {"192.168.1.10" , "192.168.1.11" , "192.168.1.12" , "192.168.1.13" , "192.168.1.14" , "192.168.1.15" , "192.168.1.16" , "192.168.1.17" , "192.168.1.18" , "192.168.1.19"};
    const char * su_ips[] = {"192.168.1.20" , "192.168.1.21" , "192.168.1.22" , "192.168.1.23" , "192.168.1.24" , "192.168.1.25" , "192.168.1.26" , "192.168.1.27" , "192.168.1.28" , "192.168.1.29" , "192.168.1.30" , "192.168.1.31"};
  

    uint32_t i = 0;
    int j = 0;
    for(i = 0 ; i < sizeof(mu_ips) / sizeof(mu_ips[0]) ; ++ i)
    {
        struct Node_Info info;
        uint32_t storage_limit = 100 * 1024 + rand() % (50 * 1024);
        info.storage_limit = storage_limit;
        int bk_nr = rand() % 6 + 5;
        for(j = 0 ; j < bk_nr ; ++ j)
        {
            Bucket_Info bk_info;
            bk_info.bucket_nr = rand() % 32;
            bk_info.bucket_load = 0;
            info.bucket_list.push_back(bk_info);
        }
        int32_t ip_int = 0;
        string_to_int_ip(mu_ips[i] , ip_int);
        RulerManager::getInstance()->m_mu_last_load.insert(make_pair(ip_int , info));
    }

    for(i = 0 ; i < sizeof(su_ips) / sizeof(su_ips[0]) ; ++ i)
    {
        struct Node_Info info;
        uint32_t storage_limit = 200 * 1024 + rand() % (50 * 1024);
        info.storage_limit = storage_limit;
        int bk_nr = rand() % 6 + 5;
        for(j = 0 ; j < bk_nr ; ++ j)
        {
            Bucket_Info bk_info;
            bk_info.bucket_nr = rand() % 64;
            bk_info.bucket_load = 0;
            info.bucket_list.push_back(bk_info);
        }
        uint32_t ip_int = 0;
        string_to_int_ip(su_ips[i] , ip_int);
        RulerManager::getInstance()->m_su_last_load.insert(make_pair(ip_int , info));
    }
}
*/

CheckHeartbeatAgent::CheckHeartbeatAgent(Epoll *epl ,
        int sec , int nsec , int timeout)
    :TimerAgent(epl , PERSIST_TIMER | CAN_REPEAT , sec , nsec)
{
    if(timeout < sec * 4)
    {
        timeout = sec * 4;
    }

    m_heartbeat_timeout = timeout;

//    test_heartbeat();
}

CheckHeartbeatAgent::CheckHeartbeatAgent(Epoll *epl , int timeout)
    :TimerAgent(epl , PERSIST_TIMER | CAN_REPEAT)
{
    if(timeout < m_expire_time.tv_sec * 4)
    {
        timeout = m_expire_time.tv_sec * 4;
    }

    m_heartbeat_timeout = timeout;

//    test_heartbeat();
}

CheckHeartbeatAgent::~CheckHeartbeatAgent()
{

}

/*  
void update_heartbeat()
{
    std::map<uint32_t , struct Node_Info>::iterator it;
    const uint32_t MB15 = 15 * 1024 * 1024;
    const uint32_t MB25 = 25 * 1024 * 1024;
    for(it = RulerManager::getInstance()->m_mu_last_load.begin() ; it != RulerManager::getInstance()->m_mu_last_load.end() ; ++ it)
    {
        struct Node_Info info = it->second;
        int bk_nr = info.bucket_list.size();
        for(int i = 0 ; i < bk_nr ; ++ i)
        {
            std::cerr<<"address : "<<&(info.bucket_list[i].bucket_nr)<<std::endl;
            std::cerr<<"bucket nr is "<<++(info.bucket_list[i].bucket_nr)<<std::endl;
        }

    }

    for(it = RulerManager::getInstance()->m_su_last_load.begin() ; it != RulerManager::getInstance()->m_su_last_load.end() ; ++ it)
    {
        struct Node_Info info = it->second;
        int bk_nr = info.bucket_list.size();
        for(int i = 0 ; i < bk_nr ; ++ i)
        {
            info.bucket_list[i].bucket_load += MB25 + rand() % (MB25);
        }
    }
}
*/

int CheckHeartbeatAgent::expire_action()
{
//    update_heartbeat();

    int ret = -1;
    int heartbeat_node = 0;

    ret = HBManager::getInstance()->check_heartbeat_timeout(m_heartbeat_timeout , heartbeat_node);
    if(ret < 0)
    {
        LOG_ERROR("CheckHeartbeatAgent::check all heartbeat timeout error !");
       return -1; 
    }
    LOG_EVENT("Check heartbeat timeout successfully ! Existing node number : "
           +int_to_str(heartbeat_node) + " AND "
            + int_to_str(ret) + " node is closed...");

    HBManager::getInstance()->display_heartbeat();

    return 0;
}


