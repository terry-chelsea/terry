#include "agent.h"
#include "Epoll.h"
#include "Server.h"
#include "debug.h"
#include "Dispatcher.h"
#include "ThreadPool.h"
#include "database.h"
#include "SocketAddress.h"
#include <cstdlib>
#include <string>
#include <signal.h>
#include <cstdio>

#define THREAD_NUMBER 32
#define MAX_CONNECTIONS 1000

DBManager *database_manager = NULL;
void init_db_connection()
{
    std::string url("mysql://root:mysql@localhost:3306/ca");
    database_manager = new DBManager();
    if(database_manager->initialize(url) < 0)
    {
        LOG_ERROR("Database pool initialize error !");
        exit(-1);
    }
    database_manager->set_max_connections(MAX_CONNECTIONS);
    std::cerr<<"max connection in thread pool : "<<database_manager->get_max_connections()
        <<std::endl;
}

#define SQL_LENGTH 1024
int check_user_table()
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

int main(int argc , char *argv[])
{
    if(argc < 2)
    {
        std::cerr<<"Usage : ./CA_login port [log_type]!"<<std::endl;;
        return -1;
    }
    
    int log_type = 0;
    if(argv[2] == NULL)
        log_type = 0;
    else 
        log_type = atoi(argv[2]);

    std::string proc = argv[0];
    proc += "_log";

    START_DEBUG(log_type , proc.c_str());

    init_db_connection();
    int ret = check_user_table();
    if(ret < 0)
    {
        LOG_ERROR("check table exist error !");
        return -1;
    }
    else if(!ret)
    {
        LOG_WARNING("User table did not exist !");
        return 0;
    }

    int port = atoi(argv[1]);
    Epoll *epl = new Epoll();
    if(epl->initialize(10000) < 0)
    {
        LOG_ERROR("epoll initialize error !");
        return -1;
    }

    SocketAddress addr("0" , port);
    Server *login = new Server(epl , ca_operatons);
    if(login->init(addr) < 0)
    {
        LOG_ERROR("Login CA server init error !");
        return -1;
    }

    if(ThreadPool::getInstancePtr()->start(THREAD_NUMBER) < 0)
    {
        LOG_ERROR("Thread pool initialize error !");
        return -1;
    }

    if(Dispatcher::getInstancePtr()->init(epl , ThreadPool::getInstancePtr()) < 0)
    {
        LOG_ERROR("Thread and main thread dispatcher init error !");
        return -1;
    }

    LOG_INFO("Start Login CA Server ...");

    while(1)
        epl->run();

    return 0;
}

