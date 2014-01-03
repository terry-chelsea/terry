/*
 * =====================================================================================
 *
 *       Filename:  HeartbeatAgent.h
 *
 *    Description:  心跳连接的时候使用这种Agent，使用长连接
 *
 *        Version:  1.0
 *        Created:  11/07/12 11:50:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_HEARTBEAT_AGENT_H_
#define _H_HEARTBEAT_AGENT_H_

#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <vector>

#include "CSTCPAgent.h"
#include "sys/time.h"
#include "RulerManager.h"
#include "../common/protocol/protocol.h"

typedef enum
{
    MU_HEARTBEAT = 0 ,
    SU_HEARTBEAT = 1 ,
    RS_HEARTBEAT = 2 ,
    UNKNOWN_HEARTBEAT = 3
}HEARTBEAT_TYPE;

static std::string type_str[] = 
{
    "MU Node" , 
    "SU Node" , 
    "RS Node" , 
    "Unkown Node"
};

class HeartbeatAgent : public CSTCPAgent
{
    public :
        HeartbeatAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);
        ~HeartbeatAgent();

        int init();

        int recycler();

        int destory();

        void readBack(InReq &req);

        HEARTBEAT_TYPE get_heartbeat_type()
        {
            return m_type;
        }

        time_t get_last_timestamp()
        {
            return m_time_stamp;
        }

        std::string get_opposite_ip()
        {
            std::string oppo_ip(this->m_Addr.getIP());
            return oppo_ip;
        }

    private :
        int do_heartbeat_handshake(HEARTBEAT_TYPE type , InReq &req);

        int do_heartbeat(HEARTBEAT_TYPE type , InReq &req);

        void update_time_stamp();

        void update_last_bucket_load(RULE_TYPE type , uint32_t ip , std::list<cstore::Bucket_Item> &bucket_list);

    private :
        uint32_t m_heartbeat_counter;
        time_t   m_time_stamp;
        HEARTBEAT_TYPE m_type;
};

#endif
