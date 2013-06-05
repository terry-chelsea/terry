#include "database.h"
#include "database/database.h"
#include "log.h"
#include "protocol.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <sys/time.h>

#define SQL_LENGTH 256
#define LOGIN_SQL "select %s , %s , %s from %s where %s= \'%s\' "
#define DATABASE_TABLE_NAME "memory_client"
#define TABLE_ID_NAME  "id"
#define TABLE_NAME_NAME "name"
#define TABLE_PASSWORD_ANAME "password"
#define TIME_NAME  "login_times"

#define MAX_CONNECTIONS 20

Database_Info::Database_Info(string url)
{
    database_manager = new DBManager();

    while(database_manager->initialize(url.c_str()) != 0);

    database_manager->set_max_connections(MAX_CONNECTIONS);
    std::cerr<<"max connection of thread pool : "<<database_manager->get_max_connections()
        <<std::endl;
}

Database_Info::~Database_Info()
{
    if(database_manager != NULL)
    {
        delete database_manager;
        database_manager = NULL;
    }
}

//#define TEST_TIME
//#define USE_SPRINTF

struct RET_INFO *Database_Info::deal_with_login(const std::string &name , int &ret)
{
#ifdef TEST_TIME
    double gap = 0.0;
    struct timeval start , end;
    gettimeofday(&start , NULL);
#endif
    DBManager *manager = database_manager;
    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        ret = -1;
        LOG_ERROR("create new database connection error !");
        return NULL;
    }

#ifdef TRANSCATION
    if(handle->begin_transaction() < 0)
    {
        ret = -1;
        LOG_ERROR("Start a new stransaction error !");
        return NULL;
    }
#endif

    struct RET_INFO *ret_info = NULL;
    long long int uid = 0;
    std::string db_password;
    std::string db_name;
    std::string sqll;
#ifdef USE_SPRINTF
    char sql[SQL_LENGTH];
    int len = snprintf(sql , SQL_LENGTH , LOGIN_SQL  ,  
            TABLE_ID_NAME , TABLE_NAME_NAME , TABLE_PASSWORD_ANAME ,  
            DATABASE_TABLE_NAME , TABLE_NAME_NAME , name.c_str());
    sql[len - 1] = '\0';
    sqll.assign(sql);
    
#else
    sqll = "select id , name , password from "DATABASE_TABLE_NAME" where name = \'" 
        + name + "\'";
#endif

    LOG_INFO("execute query sql : \"%s\"" , sqll.c_str());

    if(handle->execute_query(sqll.c_str()) < 0)
    {
        LOG_ERROR("Execute query sql : %s error !" , sqll.c_str());
        ret = -1;
#ifdef TRANSCATION
        handle->rollback_transaction();
#endif
        goto FREE;
    }
    static int counter = 0;
    if(++ counter % 9999 == 0)
        cerr<<"execute this sql "<<counter<<" times..."<<endl;

    if(!handle->next_result())
    {
        LOG_INFO("User name %s did not exist in database !" , name.c_str());
        ret = 1;
        goto FREE;
    }

    if((handle->get_long_result(1 , uid) < 0) || 
            (handle->get_string_result(2 , db_name) < 0) || 
            (handle->get_string_result(3 , db_password) < 0) )
    {
        LOG_ERROR("Get result from query result error !");
        ret = -1;
        goto FREE;
    }

    /*
#ifdef USE_SPRINTF
    memset(sql , 0 , SQL_LENGTH);
    len = snprintf(sql , SQL_LENGTH , "update %s set %s = %s +1 where %s=\'%s\' " , 
            DATABASE_TABLE_NAME , TIME_NAME , TIME_NAME , TABLE_NAME_NAME , name.c_str());
    sql[len - 1] = '\0';
    sqll.assign(sql);
#else
    
    sqll = "update "DATABASE_TABLE_NAME" set login_times = login_times +1 where name=\'" 
        + name + "\'";
#endif

    LOG_INFO("execute update sql : %s" , sqll.c_str());
    if(handle->execuate(sqll.c_str()) < 0)
    {
        LOG_ERROR("Execute uodate sql : %s error !" , sqll.c_str());
        ret = -1;
        handle->rollback_transaction();
        goto FREE;
    }
    */
#ifdef TRANSCATION
    handle->commit_transaction();
#endif

#ifdef TEST_TIME
    gettimeofday(&end , NULL);
    gap = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    gap = gap / 1000;
    cerr<<"one operation cost "<<gap<<" ms"<<endl;
#endif

    ret_info = new struct RET_INFO;
    ret_info->token = db_name;
    ret_info->pwd = db_password;
    ret_info->uid = uid;

FREE : 
    delete handle;
    handle = NULL;

    return ret_info;
}

int Database_Info::create_user_table()
{
    char sql[SQL_LENGTH];
    memset(sql , 0 , SQL_LENGTH);
    std::string table_name;
    int cnt = 0;

    int len = snprintf(sql , SQL_LENGTH , "select table_name from `INFORMATION_SCHEMA`.`TABLES` where table_name =\'%s\' and TABLE_SCHEMA=\'%s\'" , DATABASE_TABLE_NAME , "ca");
   sql[len] = '\0' ;

   LOG_INFO("sql : %s" , sql);

    DBHandle *handle = database_manager->new_connection();
    if(NULL == handle)
    {
        LOG_ERROR("create new database connection error !");
        return -1;
    }
   if(handle->execute_query(sql) < 0)
   {
       LOG_ERROR("execute SQL query line : %s error !" , sql);
       cnt = -1;
       goto RET;
   }

   while(handle->next_result())
   {
       if(handle->get_string_result(1 , table_name) < 0)
       {
           LOG_ERROR("Get string result error !");
           cnt = -1;
           goto RET;
       }
       LOG_INFO("query table name : %s" , table_name.c_str());
       cnt ++;
   }

RET : 
   if(handle != NULL)
   {
       delete handle ;
       handle = NULL;
   }

   return cnt;
}

