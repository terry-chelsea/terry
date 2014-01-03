/*
 * =====================================================================================
 *
 *       Filename:  Extent_work_item.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/12 14:50:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_EXTENT_WORK_ITEM_H_
#define _H_EXTENT_WORK_ITEM_H_

#include "WorkItem.h"
#include "DB_operation.h"

class ExtentWorkItem : public WorkItem
{
    public :
        ExtentWorkItem(RULE_TYPE type , struct Extent_Info info);
        ~ExtentWorkItem();

        int process();

        void set_operation_result(bool ret , const std::string &res)
        {
            m_extent_ret = ret ;
            m_reason = res;
        }

    private :
        int log_operation_start();

        int generate_record_to_db();

        int log_operation_end();

        int update_rule_table();

        int append_order_table();

    private :
        RULE_TYPE m_rule_type;
        struct Extent_Info m_ext_info;
        bool m_extent_ret;
        std::string m_reason;
        int m_times;
        bool m_first_ret;
        Connection_T m_conn;
        int m_operation_id;
};

#endif
