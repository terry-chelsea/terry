/*
 * =====================================================================================
 *
 *       Filename:  HB_manager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/05/13 11:01:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_HB_MANAGER_H_
#define _H_HB_MANAGER_H_

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <list>
#include "../common/sys/Singleton.h"
#include "RulerManager.h"
#include "Log_warp.h"

class HeartbeatAgent;

struct Bucket_Info
{
    uint32_t bucket_nr;
    uint32_t bucket_load;
};

struct Node_Info
{
    uint32_t storage_limit;
    std::vector<Bucket_Info> bucket_list;
};

class HBManager : public Singleton<HBManager>
{
    public : 
        HBManager();
        ~HBManager();

        //当HeartbeatAgent被创建的时候加入到链表中，定时查看链表
        void add_heartbeat_agent(HeartbeatAgent *);

        //当HeartbeatAgent析构的时候回调，两种情况：1.对端主动关闭. 2.检查心跳失效
        void heartbeat_closed(HeartbeatAgent *);

        //在心跳握手的时候初始化节点伏在信息，只设置节点的负载容量
        void init_node_heartbeat_info(RULE_TYPE , HeartbeatAgent * , uint32_t);

        //当心跳信息到达时更新节点的负载信息
        void update_last_bucket_load(RULE_TYPE , HeartbeatAgent * , const std::vector<struct Bucket_Info>&);

        //检查心跳是否超时，定时触发，第一个参数是超时时间，第二个参数是作为返回值返回
        //断开的节点数目，返回值为仍存在的节点数目
        int check_heartbeat_timeout(int , int &);

        void clear_node_info(HeartbeatAgent *);

        //显示当前仍然存在的心跳节点
        void display_heartbeat();

        bool IP_is_valid_for_migration(RULE_TYPE type , Migration_Info info);
        
        std::map<uint32_t , struct Node_Info>::iterator map_begin(RULE_TYPE type)
        {
            std::map<uint32_t , struct Node_Info>::iterator it;
            if(MU_RULER == type)
                return m_mu_last_load.begin();
            else if(SU_RULER == type)
                return m_su_last_load.begin();
            else 
            {
                LOG_ERROR("HBManager::Undefined rule type when get map begin!");
                exit(-1);
                return it;
            }
        }

        std::map<uint32_t , struct Node_Info>::iterator map_end(RULE_TYPE type)
        {
            std::map<uint32_t , struct Node_Info>::iterator it;
            if(MU_RULER == type)
                return m_mu_last_load.end();
            else if(SU_RULER == type)
                return m_su_last_load.end();
            else 
            {
                LOG_ERROR("HBManager::Undefined rule type when get map end!");
                exit(-1);
                return it;
            }
        }
        typedef std::map<uint32_t , struct Node_Info>::iterator HBItr;

    private :
        //节点负载信息队列
        std::map<uint32_t , struct Node_Info> m_mu_last_load;
        std::map<uint32_t , struct Node_Info> m_su_last_load;

        //所有心跳Agent的链表
        std::list<HeartbeatAgent *> m_heartbeat_agent_list;

        //在本次查看心跳期间断开的心跳数目
        int m_closed_agent_num;
};

#endif
