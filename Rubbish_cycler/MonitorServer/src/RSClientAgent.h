/*
 * =====================================================================================
 *
 *       Filename:  RSClientAgent.h
 *
 *    Description:  CS接收到RS拉取规则的请求的时候会创建这种Agent 
 *
 *        Version:  1.0
 *        Created:  11/07/12 11:52:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_RS_CLIENT_AGENT_H_
#define _H_RS_CLIENT_AGENT_H_

#include <iostream>
#include <string>
#include "CSTCPAgent.h"
#include "RulerManager.h"

class RSClientAgent : public CSTCPAgent
{
    public :
        RSClientAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);

        ~RSClientAgent();

        int init()
        {
//            ++ rs_request_counter;
            return 0;
        }

        int recycler()
        {
            return 0;
        }

        void readBack(InReq &req);

    private :
        int get_all_mu_rule(InReq &req);

        int get_all_su_rule(InReq &req);

        int get_mu_order_rule(InReq &req);

        int get_su_order_rule(InReq &req);
        
    private :
        //将全部的规则信息包装在一个字符串里面，使用protobuf
        int generate_all_hash_and_order_string(RULE_TYPE type , uint32_t start_version , std::string &out);
        int generate_plus_order_string(RULE_TYPE type , uint32_t start_version , std::string &out);

        //根据桶迁移或者桶扩展信息生成发送字符串
        int generate_migration_string(const Migration_Info &info , std::string &);
        int generate_extent_string(const Extent_Info &info , std::string &);

    private :
//        static uint32_t rs_request_counter;

};

//uint32_t RSClientAgent::rs_request_counter = 0;

#endif
