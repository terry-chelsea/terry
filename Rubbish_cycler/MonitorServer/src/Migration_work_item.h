/*
 * =====================================================================================
 *
 *       Filename:  Migration_work_item.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/12 14:36:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_MIGRATION_WORK_ITEM_H_
#define _H_MIGRATION_WORK_ITEM_H_

#include <iostream>
#include <cstring>
#include <string>

#include "WorkItem.h"
#include "RulerManager.h"
#include "DB_operation.h"

class MigrationWorkItem : public WorkItem
{
    public : 
        MigrationWorkItem(RULE_TYPE type , struct Migration_Info);
        ~MigrationWorkItem();

        int process();

        void set_operation_result(bool ret , const std::string &res)
        {
            m_migrate_ret = ret;
            m_reason = res;
        }

    private :
        int generate_record_to_db();

        int update_rule_table();

        int append_order_table();

        int log_operation_start();

        int log_operation_end();

    private :
        struct Migration_Info  m_mig_info;
        RULE_TYPE m_rule_type;
        bool m_first_ret;              //第一次记录的结果
        int  m_times;               //第一次只记录操作
        Connection_T m_conn;
        bool m_migrate_ret;         //执行迁移操作的结果，用于记录
        std::string m_reason;
        int m_operation_id;
};

#endif
