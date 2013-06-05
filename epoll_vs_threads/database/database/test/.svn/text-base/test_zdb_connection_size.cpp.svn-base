/*
 * =====================================================================================
 *
 *       Filename:  test_zdb_connection_size.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/13 21:09:26
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
#include <unistd.h>

#include <zdb/zdb.h>

#define URL_P "mysql:/""/root:mysql@localhost/hash"

int main()
{
    URL_T url = URL_new(URL_P);
    if(NULL == url)
    {
        std::cerr<<"url is error : "<<URL_P<<std::endl;
        return -1;
    }
    ConnectionPool_T pool = ConnectionPool_new(url);
    if(NULL == pool)
    {
        std::cerr<<"create connection pool error !"<<std::endl;
        return -1;
    }

    std::cout<<"connection number : "<<ConnectionPool_size(pool)
        <<" and active connection "<<ConnectionPool_active(pool)<<std::endl;

    ConnectionPool_start(pool);

    std::cout<<"connection number : "<<ConnectionPool_size(pool)
        <<" and active connection "<<ConnectionPool_active(pool)<<std::endl;

    Connection_T conn = ConnectionPool_getConnection(pool);
//    std::string sql = "select * from invalid";
    ResultSet_T ret = NULL;
    TRY
    {
//        ret = Connection_executeQuery(conn , "select * from %s" , "hello");
    }
    CATCH(SQLException)
    {
//        std::string err = Connection_getLastError(conn);
        std::string err = Exception_frame.message;
        std::cout<<"ERROR : "<<err<<std::endl;
        return -1;
    }
    END_TRY;

    Connection_beginTransaction(conn);
    bool err = false;
    TRY
    {
        Connection_execute(conn , "insert into mu_hash value (null , \"feng\" , null , null , null)");
        std::cout<<"insert success !"<<std::endl;
        sleep(20);
        std::cerr<<"wakeup..."<<std::endl;
        Connection_execute(conn , "update chelsea set number=1001 where id=10");
        std::cout<<"update success !"<<std::endl;
    }
    CATCH(SQLException)
        err = true;
    END_TRY;
    if(err)
    {
        std::cout<<"rollback..."<<std::endl;
        Connection_rollback(conn);
    }
    else
    {
        std::cerr<<"commit..."<<std::endl;
        Connection_commit(conn);
    }

    ConnectionPool_stop(pool);
    ConnectionPool_free(&pool);

    return 0;
}
