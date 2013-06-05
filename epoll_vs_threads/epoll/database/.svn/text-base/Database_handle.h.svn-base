/*
 * =====================================================================================
 *
 *       Filename:  Database_handle.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/12/13 01:00:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <iostream>
#include <cstring>
#include <string>
#include <list>

#include <zdb/zdb.h>

/* 每个Connection应该组合一个Result对象，因为每次查询和修改操作都会影响这个对象
 * 但是每个Connection又需要保存多个预处理对象，并且每个预处理对象有自己的Result对象
 * 每次预处理操作都创建并返回预处理对象，但是结果的查询不通过result对象，
 * 而是通过Connection或者preparement对象 , 并且预处理操作可能被用户关闭，
 * 或者链接关闭的时候回收。 */
class DBManager;
class DBPreStatementHandle;
class DBResultHandle;
class DBHandle
{
    public :
        /* create  a handle with connection ptr created by pool */
        DBHandle(DBManager * , Connection_T );
        ~DBHandle();

        /* return connection to pool */
        void close_connection();

        /* begin a transaction in this connection 
         * return 0 for success or -1 for error
         */
        int begin_transaction();

        /* commit all change since previous commit or rollback 
         * return 0 for success or -1 for error
         */
        int commit_transaction();

        /* undoes all change in this transaction , 
         * start with begin_transaction
         * return 0 for success or -1 for error
         */
        int rollback_transaction();

        /* return the value of last insert statement
         * the table must has an AUTO_INCREMENT or INTERGER primary key column!
         * return postive for success or -1 for error
         */
        int last_insert_rowId();

        /* get rows changes cased by last modify operation 
         * such as insert , update , delete 
         * return postive for success or -1 for error 
         */
        int rows_changes();

        /* execute a DDL SQL statement
         * parameters is variable , format characters
         * return -1 means error , and 0 means success
         */
        int execuate(const char * , ...);

        /* execute a select like statement
         * parameter is variable , format characters
         * query result will store in connection , 
         * result will alive until next execute or execute_query function or connection close
         * return -1 for failed while 0 for success...
         */
        int execute_query(const char * , ...);

        /* create a prepare execute SQL statement
         * return index of this prepare_statement in current connection
         * return -1 means error
         */
        DBPreStatementHandle *prepare_statement(const char * , ...);

        /* get last error of this conenction */
        std::string last_error();

        /* set waiting time of a SQL statement to finish when database is busy
         * limit is 3 seconds and parameter is in milliseconds */
        void set_query_timeout(int );

        /* get current waiting time */
        int get_query_timeout();

        /* set the limit for maximum number of rows od any Result can contain
         * if an result is exceeded this , the excess rows will dropped
         */
        void set_max_rows(int);

        /* get current maxmum rows of any result */
        int get_max_rows();

        /*---------for get result of last query statement---------------*/

        /* get next result , mive the cursor to next postion
         * NOTE:the result is initially location before the first rows , so you should
         * use this function before use result
         * return false when no more rows and true when new row is valid
         */
        bool next_result();

        /* get_***_result : get result of specific type
         * para1 is index of this cloumn
         * para2 and para3(blob) is pass bt reference to get result
         * return -1 means error and 0 means success
         */
        int get_int_result(int , int &);
        int get_string_result(int , std::string &);
        int get_long_result(int , long long int &);
        int get_double_result(int , double &);
        int get_blob_result(int , const void *& , int &);

        /* get cloumn count of last result */
        int get_cloumn_count();
        /* get cloumn name of specific index */
        int get_cloumn_name(int , std::string &);
        /* return the size of content in specific cloumn of current row */
        long get_cloumn_size(int);

        void finish_prepare(DBPreStatementHandle *);

    private :
        /* clear all result and preparedStatements */
        void clear_connection();

    private :
        Connection_T m_pconn;              //连接的句柄
        std::list<DBPreStatementHandle *> m_lpre;   //保持所有的预处理命令

        DBResultHandle *m_presult;              //该连接的查询结果，每次操作都需要更新
        DBManager   *m_pmanager;                //创建连接的连接池对象
};
