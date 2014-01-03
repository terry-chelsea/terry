/*
 * =====================================================================================
 *
 *       Filename:  SystemAgent.h
 *
 *    Description:  在系统其它节点向RS拉取规则的时候创建这种Agent 
 *
 *        Version:  1.0
 *        Created:  11/07/12 11:48:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_SYSTEM_AGENT_H_
#define _H_SYSTEM_AGENT_H_

#include <iostream>
#include <string>
#include "CSTCPAgent.h"
#include "RulerManager.h"
#include "Get_rule_task.h"

#include "../common/protocol/protocol.h"

typedef enum 
{
    GET_MU_ORDER = 0 ,
    GET_SU_ORDER = 1
}WAIT_TYPE;

class SystemAgent : public CSTCPAgent
{
    public :
        //只提供一个构造函数，这种Agent只能在对方连接的时候被动创建
        SystemAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);

        ~SystemAgent();

        //所有的请求只从缓存中读取，不会操作数据库
        void readBack(InReq &Req);

        int after_get_new_rule();

        /*  
        void writeBack(bool ret)
        {
            if(ret)
                std::cerr<<"write successed !"<<std::endl;
            else
                std::cerr<<"write failed !"<<std::endl;
        }
        */

    private :
        int get_all_mu_rule(InReq &req);
        int get_all_su_rule(InReq &req);

        int get_mu_order_rule(InReq &req);
        int get_su_order_rule(InReq &req);

        int get_mu_bucket_rule(InReq &req);
        int get_su_bucket_rule(InReq &req);

    private :

        bool judge_version(uint32_t current_version , uint32_t req_version);
        //将全部的规则信息包装在一个字符串里面，使用protobuf
        int generate_all_hash_string(RULE_TYPE type , std::string &out);
        int generate_plus_order_string(RULE_TYPE type , uint32_t start_version , std::string &out);

        //生成单个桶的规则的字符串
        int generate_single_bucket_hash_string(RULE_TYPE type , const struct HashRule & , std::string &out_data);

        //判断是发送全部的规则表还是增量的规则信息，保存在out_data中
        int send_hash_or_order_rule(RULE_TYPE type , uint32_t , std::string &out_data);

        //根据桶迁移或者桶扩展信息生成发送字符串
        int generate_migration_string(const Migration_Info &info , std::string &);
        int generate_extent_string(const Extent_Info &info , std::string &);
    
        int generate_one_order_string(RULE_TYPE type , cstore::Sys_Order *sys_order_ptr , int index);

    private :
        WAIT_TYPE  m_wait_type;
        InReq      *m_req;
};


#endif
