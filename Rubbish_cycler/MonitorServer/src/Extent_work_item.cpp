/*
 * =====================================================================================
 *
 *       Filename:  Extent_work_item.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/12 14:56:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Extent_work_item.h"
#include "Rule_file_interface.h"

#define DBP DBControl::getInstance()

ExtentWorkItem::ExtentWorkItem(RULE_TYPE type , struct Extent_Info info)
    :WorkItem() , 
    m_rule_type(type) , 
    m_ext_info(info) , 
    m_times(0) , 
    m_first_ret(false) , 
    m_conn(NULL) , 
    m_operation_id(-1)
{

}

ExtentWorkItem::~ExtentWorkItem()
{
    if(m_conn != NULL)
    {
        DBP->close_connection(m_conn);
        m_conn = NULL;
    }
}

int ExtentWorkItem::process()
{
    if(NULL == m_conn) 
    {
        Connection_T conn = DBP->create_connection();
        if(NULL == conn)
        {
            LOG_ERROR("ExtentWorkItem::create connection error !");
            m_exec_result = false;
            return -1;
        }
        else
            m_conn = conn;
    }
    
    if(0 == m_times)
    {
        ++ m_times;
        m_exec_result = true;
        if(log_operation_start() < 0)
        {
            LOG_ERROR("ExtentWorkItem::log extension start error !");
            m_first_ret = false;
        }
        else
            m_first_ret = true;

        if(generate_record_to_db() < 0)
        {
            LOG_ERROR("ExtentWorkItem::generate extension info to database error !");
            m_exec_result = false;
            return -1;
        }
        else
        {
            m_exec_result = true;
            return 0;
        }
    }
    else
    {
        if(m_first_ret)
        {
            if(log_operation_end() < 0) 
            {
                LOG_ERROR("ExtentWorkItem::log extension end error !");
            }
        }
        m_exec_result = true;
        return 0;
    }

    return 0;
}

int ExtentWorkItem::log_operation_start()
{
    std::string mod = int_to_str(m_ext_info.new_mod);
    OP_TYPE type = (MU_RULER == m_rule_type ? MU_EXTENT : SU_EXTENT);

    int id = DBP->log_op_info_start(m_conn , type , mod);
    if(id < 0)
    {
        LOG_ERROR("ExtentWorkItem::log start infomation error !");
        return -1;
    }

    m_operation_id = id;
    return 0;
}

int ExtentWorkItem::log_operation_end()
{
    if(DBP->log_op_info_end(m_conn , m_operation_id , m_extent_ret , m_reason) < 0)
    {
        LOG_ERROR("ExtentWorkItem::log end infomation error !");
        return -1;
    }

    return 0;
}

int ExtentWorkItem::generate_record_to_db()
{
    if(update_rule_table() < 0)
    {
        LOG_ERROR("ExtentWorkItem::update rule table of extension error !");
        return -1;
    }

    if(append_order_table() < 0)
    {
        LOG_ERROR("ExtentWorkItem::Append order table of extension error !");
        return -1;
    }

    return 0;
}

int ExtentWorkItem::update_rule_table()
{
    //每次只增加一倍
    int current_bucket = cs_pow(m_ext_info.new_mod - 1);

    //使用复制的语句
    std::string pre_sql = std::string("insert into ")
        + DBP->get_rule_table_name(m_rule_type) 
        + " select ? , ip0 , ip1 , ip2 , ip3 , ip4 from " 
        + DBP->get_rule_table_name(m_rule_type) 
        + " where bucket = ?";
        
    std::cerr<<"prepare sql : "<<pre_sql<<std::endl;
    PreparedStatement_T pre = DBP->prepare_execute(m_conn , pre_sql);
    if(NULL == pre)
    {
        LOG_ERROR("ExtentWorkItem::prepare statement execute sql error "
                + pre_sql);
        return -1;
    }

    int para1 = 0;
    int para2 = 0;
    for(int i = 0 ; i < current_bucket ; ++ i)
    {
        para2 = i;
        para1 = i + current_bucket;
        if((DBP->bind_int_parameter(pre , 1 , para1) < 0) || 
                DBP->bind_int_parameter(pre , 2 , para2) < 0)
        {
            LOG_ERROR("ExtentWorkItem::bind int parameter error !");
            return -1;
        }
        if(DBP->prepare_execute_operation(pre) < 0)
        {
            LOG_ERROR("ExtentWorkItem::execute prepare operation error !");
            return -1;
        }
    }

    return 0;
}

int ExtentWorkItem::append_order_table()
{
    std::string mod = int_to_str(m_ext_info.new_mod);

    std::string insert_sql = std::string("insert into ") + 
        DBP->get_order_table_name(m_rule_type) + " values (" + 
        " null , \"" + EXTENSION_COMMAND + "\" , " + mod + 
        " , null , null )";

    if(DBP->execute_operation(m_conn , insert_sql) < 0)
    {
        LOG_ERROR("ExtentWorkItem::execute insert extension info error :" 
                + insert_sql);
        return -1;
    }

    return 0;
}
