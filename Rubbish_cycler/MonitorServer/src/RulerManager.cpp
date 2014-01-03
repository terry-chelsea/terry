/*
 * =====================================================================================
 *
 *       Filename:  RulerManager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 14:31:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "RulerManager.h"
#include "Log_warp.h"
#include "HeartbeatAgent.h"

//初始化规则表，增量规则表需要包含一个元素，桶规则表为空
int RulerManager::init_rule()
{
    OrderRule order_rule;
    memset(&order_rule , 0 , sizeof(order_rule));
    order_rule.cmd = UNDEFINED;
    order_rule.version = 0;

    m_su_order_table.reserve(INIT_VEC_SIZE);
    m_su_order_table[0] = order_rule;

    m_mu_order_table.reserve(INIT_VEC_SIZE);
    m_mu_order_table[0] = order_rule;

    m_mu_hash_table.reserve(INIT_VEC_SIZE);
    m_su_hash_table.reserve(INIT_VEC_SIZE);
    return 0;
}

//检查防止错误发生，如果不相同，必须退出.
int RulerManager::get_hash_rule(uint32_t bucket_nr , RULE_TYPE type , struct HashRule &hash_rule)
{
    //上层调用保证输入的规则是MU或者SU
   if(bucket_nr < 0 || bucket_nr >= m_current_bucket_number[type])
   {
       LOG_ERROR("RulerManager::Invalid Input : current bucket mod : " 
               + int_to_str(m_current_mod[type]) + " Reuqest bucket : " + int_to_str(bucket_nr));
       return -1;
   }
   if(MU_RULER == type)
   {
        if(m_mu_hash_table[bucket_nr].bucket_nr != bucket_nr)
        {
            LOG_ERROR("RulerManager::mu rule error : hash bucket : " + int_to_str(m_mu_hash_table[bucket_nr].bucket_nr) 
                    + " Requesr bucket : " + int_to_str(bucket_nr));
            set_dead_line("MU Hash rule table error !");
        }
        hash_rule = m_mu_hash_table[bucket_nr];
   } 
   if(SU_RULER == type)
   {
        if(m_su_hash_table[bucket_nr].bucket_nr != bucket_nr)
        {
            LOG_ERROR("RulerManager::mu rule error : hash bucket : " + int_to_str(m_mu_hash_table[bucket_nr].bucket_nr) 
                    + " Requesr bucket : " + int_to_str(bucket_nr));
            set_dead_line("SU Hash rule table error !");
        }
        hash_rule = m_su_hash_table[bucket_nr];
   }

   return 0;
}

int RulerManager::get_order_rule(uint32_t version , RULE_TYPE type , struct OrderRule &order_rule)
{
    if(version <= 0 || version > m_current_version[type])
    {
        LOG_ERROR("RulerManager::Invalid Input : current version : " 
                + int_to_str(m_current_version[type]) + " Request Version : " + int_to_str(version));
        return -1;
    }

    if(MU_RULER == type)
    {
        if(m_mu_order_table[version].version != version)
        {
            LOG_ERROR("RulerManager::mu rule error : order version : " + int_to_str(m_mu_order_table[version].version) 
                    + " Request version : " + int_to_str(version));
            set_dead_line("MU Order rule table error !");
        }
        order_rule = m_mu_order_table[version];
    }
    if(SU_RULER == type)
    {
        if(m_su_order_table[version].version != version)
        {
            LOG_ERROR("RulerManager::mu rule error : order version : " + int_to_str(m_su_order_table[version].version) 
                    + " Request version : " + int_to_str(version));
            set_dead_line("SU Order rule table error !");
        }
        order_rule = m_su_order_table[version];
    }

    return 0;
}

//传入的信息是具体的IP地址和桶号，而不是一个MIgration_Info对象
void RulerManager::new_migration_order(uint32_t bucket_nr , uint32_t src_ip , uint32_t dest_ip , RULE_TYPE type)
{
    struct OrderRule order_rule;
    memset(&order_rule , 0 , sizeof(order_rule));
    struct Migration_Info migration;
    memset(&migration , 0 , sizeof(migration));

    migration.bucket_nr = bucket_nr;
    migration.src_ip = src_ip;
    migration.dest_ip = dest_ip;

    //更新全局规则表的信息,该函数正确返回保证参数的正确性
    if(modify_rule_by_migration(migration , type) < 0)
    {
        LOG_ERROR("RulerManager::Modify rule by Migration error : Inside Error !");
        return ;
    }

    order_rule.cmd = MIGRATION;
    order_rule.version = m_current_version[type] + 1;
    order_rule.order.migration = migration;

    //只有在成功的时候才会更新增量规则，更新增量的规则表信息
    add_new_order_item(order_rule , MU_RULER);
    
    //增加版本号
    m_current_version[type] ++ ;
}

int RulerManager::modify_rule_by_migration(struct Migration_Info info , RULE_TYPE type)
{
    uint32_t bucket_nr = info.bucket_nr;
    struct HashRule rule_item;
    memset(&rule_item , 0 , sizeof(rule_item));
    uint32_t ip_temp = 0;
    bool flag = false;

    if(SU_RULER == type)
    {
        //找到桶号的桶规则信息
        rule_item = m_su_hash_table[bucket_nr];
        if(rule_item.bucket_nr != bucket_nr)
        {
            LOG_ERROR("RulerManager::SU Rule is Wrong : bucket : " + int_to_str(bucket_nr));
            set_dead_line("SU Hash Rule Table Error !");
            return -1;
        }
    }
    else if(MU_RULER == type)
    {
        rule_item = m_mu_hash_table[bucket_nr];
        if(rule_item.bucket_nr != bucket_nr)
        {
            LOG_ERROR("RulerManager::MU Rule is Wrong : bucket : " + int_to_str(bucket_nr));
            set_dead_line("SU Hash Rule Table Error !");
        }
    }

    for(int i = 0 ; i < MAX_IP_NUM ; ++ i)
    {
        ip_temp = rule_item.ip[i];
        if(INVALID_IP == ip_temp)
            break ;

        if(ip_temp == info.src_ip)
        {
//            std::cerr<<"-----------------here modify source IP !----------------"<<std::endl;
            //将该桶的节点地址改变为目的地址的IP地址
            rule_item.ip[i] = info.dest_ip;
                flag = true;
        }
    }

    //没有找到这个桶对于的源IP地址，出现错误.
    if(!flag)
    {
        std::string ip_str;
        int_to_string_ip(info.src_ip , ip_str);
        LOG_ERROR("RulerManager::SU Rule is Wrong : bucket : " 
                + int_to_str(bucket_nr) + " srouce IP : " + ip_str);
        return -1;
    }
    else
    {
        if(MU_RULER == type)
            m_mu_hash_table[bucket_nr] = rule_item;
        else
            m_su_hash_table[bucket_nr] = rule_item;

    }

    return 0;
}

void RulerManager::new_extent_order(RULE_TYPE type) 
{
    struct OrderRule order_rule;
    memset(&order_rule , 0 , sizeof(order_rule));
    struct Extent_Info extent;
    memset(&extent , 0 , sizeof(extent));

    extent.new_mod = m_current_mod[type] + 1;

    //更新全局规则表的信息,该函数正确返回保证参数的正确性
    if(modify_rule_by_extent(extent , type) < 0)
    {
        LOG_ERROR("RulerManager::Modify rule by Extent error : Inside Error !");
        return ;
    }
    
    order_rule.cmd = EXTENT;
    order_rule.version = m_current_version[type] + 1;
    order_rule.order.extent = extent;

    add_new_order_item(order_rule , type);

    m_current_version[type] ++;
    m_current_mod[type] ++ ;
    m_current_bucket_number[type] *= 2;
}

//不需要再查看type的类型了
int RulerManager::modify_rule_by_extent(struct Extent_Info info , RULE_TYPE type)
{
    int bucket_num = m_current_bucket_number[type];
    struct HashRule hash_rule;
    memset(&hash_rule , 0 , sizeof(hash_rule));

    if(SU_RULER == type)
    {
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            hash_rule = m_su_hash_table[i];
            //只需要修改桶号，IP地址不需要改变
            hash_rule.bucket_nr = i + bucket_num;
            add_new_hash_item(hash_rule , SU_RULER);
        }
    }
    if(MU_RULER == type)
    {
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            hash_rule = m_mu_hash_table[i];
            //只需要修改桶号，IP地址不需要改变
            hash_rule.bucket_nr = i + bucket_num;
            m_mu_hash_table.push_back(hash_rule);
            add_new_hash_item(hash_rule , MU_RULER);
        }
    }

    return 0;
}

//根据规则项的桶号或者规则的版本号设置，首先判断当前的容量，如果查出，需要再分配。
void RulerManager::add_new_hash_item(struct HashRule rule , RULE_TYPE type)
{
    uint32_t index = rule.bucket_nr;
    if(MU_RULER == type)
    {
        if(m_mu_hash_table.capacity() <= index)
        {
            m_mu_hash_table.reserve(index * 2 + 1);
        }
        m_mu_hash_table[index] = rule;
    }
    
    if(SU_RULER == type)
    {
        if(m_su_hash_table.capacity() < index)
        {
            m_su_hash_table.reserve(index * 2 + 1);
        }
        m_su_hash_table[index] = rule;
    }
}

void RulerManager::add_new_order_item(struct OrderRule rule , RULE_TYPE type)
{
    uint32_t index = rule.version;
    if(MU_RULER == type)
    {
        if(m_mu_order_table.capacity() <= index)
        {
            m_mu_order_table.reserve(index * 2 + 1);
        }
        m_mu_order_table[index] = rule;
    }

    if(SU_RULER == type)
    {
        if(m_su_order_table.capacity() <= index)
        {
            m_su_order_table.reserve(index * 2 + 1);
        }
        m_su_order_table[index] = rule;
    }
}

void RulerManager::set_order_rule(uint32_t version , OP_CMD cmd , std::string content , RULE_TYPE type)
{
    OrderRule order_rule;
    order_rule.version = version;
    order_rule.cmd = cmd;

    if(MIGRATION == cmd)
    {
        if(content.size() < sizeof(Migration_Info))
        {
            LOG_ERROR("RulerManager::Invalid Input : migration content : " + content);
            return ;
        }

        Migration_Info *info = (Migration_Info *)(content.c_str());
        order_rule.order.migration = *info;
    }
    if(EXTENT == cmd)
    {
        if(content.size() < sizeof(Extent_Info))
        {
            LOG_ERROR("RulerManager::Invalid Input : extent content : " + content);
            return ;
        }

        Extent_Info *info = (Extent_Info *)(content.c_str());
        order_rule.order.extent = *info;
    }

    add_new_order_item(order_rule , type);
}

void RulerManager::set_hash_rule(uint32_t bucket_nr , uint32_t *ip_array , int num , RULE_TYPE type)
{
    if(ip_array == NULL || num > MAX_IP_NUM)
    {
        LOG_ERROR("RulerManager::Invalid Input : error set IP array !");
        return ;
    }

    struct HashRule hash_rule;
    memset(&hash_rule , 0 , sizeof(hash_rule));
    hash_rule.bucket_nr = bucket_nr;
    for(int i = 0 ; i < MAX_IP_NUM ; ++ i)
    {
        if(i < num)
            hash_rule.ip[i] = ip_array[i];
        else
            hash_rule.ip[i] = INVALID_IP;
    }
    
    add_new_hash_item(hash_rule , type);
}

