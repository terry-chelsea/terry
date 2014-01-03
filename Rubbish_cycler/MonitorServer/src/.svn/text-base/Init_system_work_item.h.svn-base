/*
 * =====================================================================================
 *
 *       Filename: .h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/13 18:26:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_INIT_SYSTEM_WORK_ITEM_H
#define _H_INIT_SYSTEM_WORK_ITEM_H

#include <iostream>
#include <string>
#include <vector>
#include "DB_operation.h"
#include "WorkItem.h"

struct All_Init_info
{
    std::vector<struct HashRule> rule_table;
    int dup;
    int mod;
};

class InitSystemWorkItem : public WorkItem
{
    public : 
        InitSystemWorkItem(RULE_TYPE type , struct All_Init_info v1)
            :m_init_info(v1) ,
            m_rule_type(type)
    {

    }

        ~InitSystemWorkItem()
        {}

        int process();

    private :
        //在系统部署的时候调用该接口，根据部署情况初始化数据库
        int init_cstore_database(RULE_TYPE , const std::vector<struct HashRule> & , int , int);

        int init_tables(Connection_T , RULE_TYPE);

        int generate_every_bucket_rule(PreparedStatement_T , struct HashRule);

        int log_init_info(Connection_T , RULE_TYPE , int , int);
        
        std::string init_info_string();

        int init_operation_table(Connection_T);

    private :
        struct All_Init_info m_init_info;
        RULE_TYPE m_rule_type;
};

#endif
