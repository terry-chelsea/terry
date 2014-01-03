/*
 * =====================================================================================
 *
 *       Filename:  MPCClientAgent.h
 *
 *    Description:  MPC请求到来的时候创建这种Agent
 *
 *        Version:  1.0
 *        Created:  11/07/12 11:46:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_MPC_CLIENT_AGENT_H_
#define _H_MPC_CLIENT_AGENT_H_

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <list>
#include <sys/time.h>
#include "CSTCPAgent.h"
#include "RulerManager.h"
#include "../common/protocol/protocol.h"
#include "Migration_task.h"
#include "Init_system_task.h"

class MPCClientAgent : public CSTCPAgent
{
    public :
        MPCClientAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl);

        ~MPCClientAgent();

        void readBack(InReq &req);

        void do_migrate_callback(bool ret , RULE_TYPE type , Migration_Info info);
        void do_extent_callback(bool ret , RULE_TYPE type , Extent_Info info);
        void after_init_done(bool ret , RULE_TYPE type);

    private :
        int do_rubbish_recycle(InReq &Req);
        int do_migrate_bucket(RULE_TYPE type , InReq &req);
        int do_extent_bucket(RULE_TYPE type , InReq &Req);
        int do_init_hash_rule(RULE_TYPE type , InReq &Req);
        int do_get_last_bucket_info(RULE_TYPE type , InReq &req);
        int do_get_hash_rule(RULE_TYPE type , InReq &req);
        int do_global_bucket_info(InReq &req);

        void hash_item_to_map_item(cstore::Map_Item * , const HashRule &rule_item);
        void bucket_info_to_bucket_list(cstore::Bucket_List *bucket_list , const vector<Bucket_Info> &info);

        int init_globle_hash_rule(RULE_TYPE , const vector<cstore::IP_scope> & , int , int , std::vector<uint32_t> &);

    private :
        MigrationTask *m_migrate_task;
        InitSystemTask *m_init_task;
};


#endif
