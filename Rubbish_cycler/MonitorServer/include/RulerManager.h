/*
 * =====================================================================================
 *
 *       Filename:  RulerManager.h
 *
 *    Description:  管理所有的规则的缓存和各个节点的负载信息
 *
 *        Version:  1.0
 *        Created:  11/07/12 12:01:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_RULER_NAMAGER_H_
#define _H_RULER_NAMAGER_H_

#include<vector>
#include<map>
#include<string>
#include<iostream>
#include<stdint.h>
#include "../common/sys/Singleton.h"
#include "Lib_tool.h"

//规则表使用数组的方式存放，这是基于所有的桶号是从0开始的，所有的版本号也是顺序递增的。
//但是能够保证桶号是从0开始的，一次递增的，所以可以随机访问
//但是增量的版本号却是从1开始的，这样只好在版本号数组下表为0的地方设置为一个无效值
//对输入的参数需要进行检验
//因此需要保证初始化的时候桶号是从0开始的，增量规则初始化的时候下标为0填充一个无效值

#define MAX_IP_NUM  5
#define INVALID_IP  0
#define INIT_VEC_SIZE 16

uint32_t const RS_SYS_UPDATE_ORDER = 0;
uint32_t const RS_SYS_UPDATE_ALL_HASH = 1;

const uint32_t MPC_CS_MIGRATE_SUCCESS = 0;
const uint32_t MPC_CS_MIGRATE_FAILED = 1;
const uint32_t MPC_CS_EXTENT_SUCCESS = 2;
const uint32_t MPC_CS_EXTENT_FAILED = 2;

const uint32_t NEW_MU_RULE  =  0x1;
const uint32_t NEW_SU_RULE  =  0x2;

typedef enum 
{
    MIGRATION = 0 ,
    EXTENT = 1 ,
    UNDEFINED = 2

}OP_CMD;

struct HashRule
{
    uint32_t bucket_nr;
    uint32_t ip[MAX_IP_NUM];
};

struct Migration_Info
{
    uint32_t bucket_nr;
    uint32_t src_ip;
    uint32_t dest_ip;
};

struct Extent_Info
{
    uint32_t new_mod;
};

struct OrderRule
{
    OP_CMD cmd;
    uint32_t version;

    union 
    {
        struct Migration_Info migration;
        struct Extent_Info    extent;
    }order;
};

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

typedef enum 
{
    MU_RULER = 0 ,
    SU_RULER = 1
}RULE_TYPE;


class RulerManager : public Singleton<RulerManager>
{
    friend class Singleton<RulerManager> ;

    public : 
        RulerManager()
        {
            m_current_version[0] = 0;
            m_current_version[1] = 0;
            m_current_mod[0] = 0;
            m_current_mod[1] = 0;
            m_current_bucket_number[0] = 0;
            m_current_bucket_number[1] = 0;
        }

        ~RulerManager()
        {
        }

        uint32_t get_current_version(RULE_TYPE type)
        {
            return m_current_version[type];
        }

        uint32_t get_current_mod(RULE_TYPE type)
        {
            return m_current_mod[type];
        }

        uint32_t get_bucket_number(RULE_TYPE type)
        {
            return m_current_bucket_number[type];
        }

        void set_current_version(RULE_TYPE type , uint32_t new_version)
        {
            m_current_version[type] = new_version;
        }
        void set_current_mod(RULE_TYPE type , uint32_t new_mod)
        {
            m_current_mod[type] = new_mod;
            m_current_bucket_number[type] = cs_pow(new_mod);
        }

        //初始化 规则表的缓存
        int init_rule();
        
        //读取规则表中的一项
        int get_hash_rule(uint32_t bucker_nr , RULE_TYPE type , struct HashRule &);

        //读取更新的规则版本库中的一项
        int get_order_rule(uint32_t version , RULE_TYPE type , struct OrderRule &);

        //获取单个SU节点的负载信息
        int get_su_bucket_load(uint32_t , std::vector<struct Bucket_Info> &info);
       
        //获取单个MU节点的伏在信息
        int get_mu_bucket_load(uint32_t , std::vector<struct Bucket_Info> &info);
        
        //一次桶迁移操作多规则表的改变情况
        void new_migration_order(uint32_t bucket_nr , uint32_t src_ip , uint32_t dest_ip , RULE_TYPE type);

        //假设每次桶扩展都只会将模值增加1
        void new_extent_order(RULE_TYPE type);

        void add_new_order_item(struct OrderRule , RULE_TYPE);

        void add_new_hash_item(struct HashRule , RULE_TYPE type);

        //根据心跳更新一个SU节点的负载信息
        int set_su_load(uint32_t , const std::vector<struct Bucket_Info> &info);

        //根据心跳信息更新MU节点的负载信息
        int set_mu_load(uint32_t , const std::vector<struct Bucket_Info> &info);

        //初始化的时候登记每个节点的信息
        void init_node_load_list(RULE_TYPE type , uint32_t ip , uint32_t load_limit);

        /*
        //如果只是一个RS模块，判断当前RS是否正在向CS拉取规则
        void rs_start_get_rule(RULE_TYPE type)
        {
            if(MU_RULER == type)
                m_rs_getting_mu_rule = true ;
            else 
                m_rs_getting_su_rule = true ;
        }
        */

        /*  
        //设置一个RS的状态，在向CS拉取规则的时候设置
        void rs_get_rule_back(RULE_TYPE type)
        {
            if(MU_RULER == type)
                m_rs_getting_mu_rule = false ;
            else 
                m_rs_getting_su_rule = false ;
        }

        bool rs_is_getting_rule(RULE_TYPE type)
        {
            if(MU_RULER == type)
                return m_rs_getting_mu_rule ;
            else 
                return m_rs_getting_su_rule ;
        }
        */

        //初始化规则表的时候使用，设置一个桶的规则表信息
        void set_hash_rule(uint32_t bucket_nr , uint32_t *ip_array , int num , RULE_TYPE type);
        
        //初始化规则表的时候使用，设置一个规则版本的信息
        void set_order_rule(uint32_t version , OP_CMD cmd , std::string content , RULE_TYPE type);

        //根据一个节点的IP判断它是哪一种节点，MU or SU or RS...
        int get_node_type(uint32_t ip);

        void node_is_closed(RULE_TYPE type , uint32_t ip);

        bool IP_is_valid_for_migration(RULE_TYPE type , Migration_Info info);

    private :
        //根据一次桶迁移操作对全局的规则表进行适当的改变
        int modify_rule_by_migration(struct Migration_Info , RULE_TYPE type);

        //根据一次桶扩展操作对全局的规则表进行适当的改变
        int modify_rule_by_extent(struct Extent_Info , RULE_TYPE type);

    private : 
        uint32_t m_current_version[2];
        uint32_t m_current_mod[2];
        uint32_t m_current_bucket_number[2];
//        uint32_t m_su_copy_num;

        /*  
        //RS当前是否正在从CS拉取规则
        bool  m_rs_getting_mu_rule;
        bool  m_rs_getting_su_rule;
        */

    private :
        std::vector<struct HashRule> m_mu_hash_table;
        std::vector<struct HashRule> m_su_hash_table;
        std::vector<struct OrderRule> m_mu_order_table;
        std::vector<struct OrderRule> m_su_order_table;

    public :
        std::map<uint32_t , struct Node_Info> m_mu_last_load;
        std::map<uint32_t , struct Node_Info> m_su_last_load;

};

#endif
