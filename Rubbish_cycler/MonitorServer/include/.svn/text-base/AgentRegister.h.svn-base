/*
 * =====================================================================================
 *
 *       Filename:  AgentRegister.h
 *
 *    Description:  为每一个Agent注册一个记录号，根据记录号查找Agent，而不是直接引用。
 *
 *        Version:  1.0
 *        Created:  11/07/12 10:02:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_AGENT_REGISTER_H_
#define _H_AGENT_REGISTER_H_

#include <iostream>
#include <list>
#include <map>

#include "sys/time.h"
#include "../common/sys/Singleton.h"
#include "../common/comm/comm.h"
#include "HeartbeatAgent.h"
#include "CSTCPAgent.h"

class SCAgent;

//为每一个Agent分配一个Agent编号，32位无符号整数，这样可以假设不会有任何两个Agent有相同的编号
class AgentRegister : public Singleton<AgentRegister>
{
    friend class Singleton<AgentRegister> ;
    public : 
        AgentRegister();
        ~AgentRegister();

        //创建一个通用的短连接Agent
        SCAgent* create_SC_agent(const SocketAddress &addr , Epoll *epl);

        //根据agent的index号移除这个agent
        void delete_SC_agent(uint32_t agent_index);

        //删除对于的agent
        void recycler_SC_agent();

        uint32_t add_agent(CSTCPAgent *);

        void delete_agent(uint32_t index);

        CSTCPAgent* get_agent_from_index(uint32_t index);

        void generate_index(uint32_t &index);

        void add_heartbeat_agent(HeartbeatAgent *);

        void delete_heartbeat_agent(HeartbeatAgent *);

        void heartbeat_is_closed(std::string ip);

        void heartbeat_is_checked();

        void add_special_agent(CSTCPAgent *);

        void delete_special_agent(uint32_t index);

        int check_all_heartbeat_timeout(int timeout , int &num);

    private :
        uint32_t m_next_agent_index;
        //所有心跳连接Agent的链表
        std::list<HeartbeatAgent *> m_heartbeat_agent_list;
        std::list<std::string> m_closed_heartbeat_list;
        std::map<uint32_t , CSTCPAgent *> m_normal_agent_map;
        std::list<CSTCPAgent *> m_special_agent_list;

        std::list<CSTCPAgent *> m_recycler_agent_list;

};

#endif
