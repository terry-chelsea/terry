/*
 * =====================================================================================
 *
 *       Filename:  Migration_work_item.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/12 14:40:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Migration_work_item.h"
#include "Rule_file_interface.h"
#include "CSConfiguration.h"

#define DBP DBControl::getInstance()
#define insert_prefix  "insert into "
#define insert_mid  " values "
extern const char *op_types[];

MigrationWorkItem::MigrationWorkItem(RULE_TYPE type , struct Migration_Info info)
    :WorkItem() ,
    m_mig_info(info) ,
    m_rule_type(type) , 
    m_first_ret(false) , 
    m_times(0) , 
    m_conn(NULL) , 
    m_migrate_ret(false) ,
    m_operation_id(-1)
{

}

MigrationWorkItem::~MigrationWorkItem()
{
    if(m_conn != NULL)
    {
        DBP->close_connection(m_conn);
        m_conn = NULL;
    }    
}

int MigrationWorkItem::process()
{
    //只创建 一次数据连接句柄
    if(m_conn == NULL)
    {
        Connection_T conn = DBP->create_connection();
        if(NULL == conn)
        {
            LOG_ERROR("MigrationWorkItem::create new connection error !");
            m_exec_result = false;
            return -1;
        }
        else
            m_conn = conn;
    }

    //第一次只记录迁移操作
    if(0 == m_times)
    {
        ++ m_times;
        m_exec_result = true;
        if(log_operation_start() < 0)
        {
            LOG_ERROR("MigrationWorkItem::log migration operation start error !");
            m_first_ret = false;
            return -1;
        }
        else
        {
            m_first_ret = true;
            return 0;
        }
    }

    if(generate_record_to_db() < 0)
    {
        LOG_ERROR("MigrationWorkItem::generate migration info to database error !");
//        goto ERR;
        m_exec_result = false;
        return -1;
    }

    m_exec_result = true;
    return 0;

    /* 
ERR :
    m_exec_result = false;
    return -1;
    */
}

int MigrationWorkItem::generate_record_to_db()
{
    bool db_op_ret = false;
    //只有在第一步操作成功完成的情况下才修改数据库
    if(m_migrate_ret)
    {
        if(update_rule_table() < 0)
        {
            LOG_ERROR("MigrationWorkItem::Update rule table of migration error !");
            return -1;
        }

        if(append_order_table() < 0)
        {
            LOG_ERROR("MigrationWorkItem::Append order table of migration error !");
            return -1;
        }
    }

    //只有在start处理成功的时候才会记录end
    if(m_first_ret)
    {
        if(log_operation_end() < 0)
        {
            LOG_ERROR("MigrationWorkItem::Log migration operation error !");
        }
    }

    return 0;
}

int MigrationWorkItem::update_rule_table()
{
    std::string select_sql = std::string("select * from ") 
        + DBP->get_rule_table_name(m_rule_type)
        + " where bucket = " + int_to_str(m_mig_info.bucket_nr);

    ResultSet_T ret = DBP->execute_query(m_conn , select_sql);
    if((NULL == ret) || (!ResultSet_next(ret)))
    {
        LOG_ERROR("MigrationWorkItem::execute query sql error : " + select_sql);
        return -1;
    }
    std::string ip_str;
    if(int_to_string_ip(m_mig_info.src_ip , ip_str) < 0)
    {
        LOG_ERROR("MigrationWorkItem::change int to string ip error : " 
                + int_to_str(m_mig_info.src_ip));
        return -1;
    }

    int i = 0;
    for(i = 0 ; i < MAX_IP_NUM ; ++ i)
    {
        std::string ret_ip;
        if(DBP->get_string_result(ret , i + 2 , ret_ip) < 0)
        {
            LOG_ERROR("MigrationWorkItem::get string result error !");
            return -1;
        }
        if(!ret_ip.compare(ip_str))
        {
            break;
        }
    }
    if(MAX_IP_NUM == i)
    {
        LOG_ERROR("MigrationWorkItem::Can not find source IP in database !");
        return -1;
    }

    if(int_to_string_ip(m_mig_info.dest_ip , ip_str) < 0)
    {
        LOG_ERROR("MigrationWorkItem::change int to string ip error : " 
                + int_to_str(m_mig_info.dest_ip));
        return -1;
    }

    //真TMD低端，怎么方便实现！！！
    std::string update_sql = std::string("update ") 
        + DBP->get_rule_table_name(m_rule_type)
        + " set ip" + int_to_str(i) + " = " + "\"" + ip_str + "\"" 
        + "where bucket = " + int_to_str(m_mig_info.bucket_nr);

    if(DBP->execute_operation(m_conn , update_sql) < 0)
    {
        LOG_ERROR("MigrationWorkItem::update rule table error !");
        return -1;
    }

    return 0;
}

int MigrationWorkItem::append_order_table()
{
    //this is ugly...
    std::string bucket_nr = int_to_str(m_mig_info.bucket_nr);
    std::string src_ip;
    std::string dest_ip;
    if((int_to_string_ip(m_mig_info.src_ip , src_ip) < 0) ||
            int_to_string_ip(m_mig_info.dest_ip , dest_ip) < 0)
    {
        LOG_ERROR("MigrationWorkItem::int ip to string error !");
        return -1;
    }

    std::string insert_sql = std::string("insert into ") + 
        DBP->get_order_table_name(m_rule_type) + " values (" + 
        "null" + " , \"" + MIGRATION_COMMAND + "\" , " + bucket_nr + 
        " , \"" + src_ip + "\" , \"" + dest_ip + "\")";

    std::cerr<<"insert sql : "<<insert_sql<<std::endl;

    if(DBP->execute_operation(m_conn , insert_sql) < 0)
    {
        LOG_ERROR("MigrationWorkItem::execute insert migration info error : " 
                + insert_sql);
        return -1;
    }

    return 0;
}

int MigrationWorkItem::log_operation_start()
{
    std::string src_ip , dest_ip;
    if((int_to_string_ip(m_mig_info.src_ip , src_ip) < 0) ||
            int_to_string_ip(m_mig_info.dest_ip , dest_ip) < 0)
    {
        LOG_ERROR("MigrationWorkItem::int ip to string error when log!");
        return -1;
    }

    std::string info = int_to_str(m_mig_info.bucket_nr) + ":" 
        + src_ip + ":" + dest_ip;
    OP_TYPE type = (MU_RULER == m_rule_type ? MU_MIGRATION : SU_MIGRATION);

    int id = DBP->log_op_info_start(m_conn , type , info);
    if((id < 0))
    {
        LOG_ERROR("MigrationWorkItem::log start infomation error !");
        return -1;
    }
    m_operation_id = id;

    return 0;
}

int MigrationWorkItem::log_operation_end()
{
    if(DBP->log_op_info_end(m_conn , m_operation_id , m_migrate_ret , m_reason) < 0)
    {
        LOG_ERROR("MigrationWorkItem::log end infomation error !");
        return -1;
    }

    return 0;
}
