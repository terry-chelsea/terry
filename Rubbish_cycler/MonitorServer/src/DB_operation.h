/*
 * =====================================================================================
 *
 *       Filename:  DB_operation.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/13 11:51:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "zdb.h"
#include <iostream>
#include <cstdlib>
#include <string>

#include "../common/sys/Singleton.h"
#include "RulerManager.h"

#ifndef _H_DB_OPERATION_H_
#define _H_DB_OPERATION_H_

typedef enum 
{
    MU_MIGRATION , //MU的桶迁移操作
    MU_EXTENT ,    //MU的桶扩展
    SU_MIGRATION , //SU的桶迁移
    SU_EXTENT ,    //SU的桶扩展
    RUBBISH_RECYCLER ,  //全局的垃圾回收
    MU_INIT_DEPLOY ,     //初始化部署系统
    SU_INIT_DEPLOY ,     //初始化部署系统
    MAX_TYPE        //无效值
}OP_TYPE;

#define INVALID_IP_STR "0"
#define MIGRATION_COMMAND "migration"
#define EXTENSION_COMMAND "extension"


class DBControl : public Singleton<DBControl>
{
    public:
        DBControl();
        ~DBControl();

        //初始化连接池等资源信息
        int init_dbcontrol(const std::string &);

        //释放连接池等资源信息
        void destory_dbcontrol();

        //创建一个连接
        Connection_T create_connection();

        //关闭一个连接
        void close_connection(Connection_T conn);

        //执行一条数据库修改的操作，例如insert、create、update等操作
        int execute_operation(Connection_T , const std::string &);

        //执行一条查询操作，例如select , 返回一条查询的结果，如果结果为多条，需要循环调用ResultSet_next，直到函数返回值为NULL。
        ResultSet_T execute_query(Connection_T , const std::string &);

        //执行一条预处理，如果出错返回NULL，成功返回预处理句柄
        PreparedStatement_T prepare_execute(Connection_T , const std::string &);

        //绑定一个int类型的参数
        int bind_int_parameter(PreparedStatement_T , int , int);

        //绑定一个string类型的参数
        int bind_string_parameter(PreparedStatement_T , int , const std::string &);

        //绑定一个二进制类型的参数
        int bind_blob_parameter(PreparedStatement_T , int , void * , int);

        int get_int_result(ResultSet_T , int);

        int get_string_result(ResultSet_T , int , std::string &);

        //执行一条预处理修改操作
        int prepare_execute_operation(PreparedStatement_T);

        //执行一条预处理查询操作
        ResultSet_T prepare_execute_query(PreparedStatement_T);

        //在执行操作之前需要记录当前操作到数据库，参数有类型和负载数据，返回的为id，出错为-1
        int log_op_info_start(Connection_T , OP_TYPE , const std::string &);

        //在执行操作完成之后记录操作结果到数据库，参数有该项的id，是否成功和附加信息.
        int log_op_info_end(Connection_T , int , bool , const std::string &);

        std::string get_rule_table_name(RULE_TYPE );
        std::string get_order_table_name(RULE_TYPE );

    private :
        URL_T m_url_handler;
        ConnectionPool_T m_connpool;
        bool m_binit;
};


#endif
