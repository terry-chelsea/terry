/*
 * =====================================================================================
 *
 *       Filename:  manager_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 16:14:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "database.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <pthread.h>
#include <sys/time.h>

using namespace std;

const char *url_line = "mysql:/""/root:mysql@localhost:3306/terry"

int main(int argc , char *argv[])
{
    if(argc != 2)
    {
        cerr<<"Uasge : ./test connections\n";
        return -1;
    }

    DBManager *manager = new DBManager();
    if(manager->initialize(url_line) < 0)
    {
        std::cerr<<"initialize database manager failed !"<<endl;
        return -1;
    }

    srand(time(NULL));
    int conn_num = atoi(argv[1]);
    if(conn_num > 30 || conn_num < 0)
        conn_num = 6;
    
    pthread_t *tid = new pthread_t[conn_num];
    for(int i = 0 ; i < conn_num ; ++ i)
    {
        if(pthread_create(tid + i , NULL , func , (void *)manager) < 0)
        {
            perror("create new thread error : ");
            return -1;
        }
    }


    for(int i = 0 ; i < conn_num ; ++ i)
    {
        pthread_join(tid[i] , NULL);
    }

    cerr<<"all job is done ..."<<endl;
    return 0;
}

#define TIMES_PER_THREAD 100
static 

void *func(void *args)
{
    DBManager *manager = (DBManager *)args;

    int times = srand() % 100 + 100;
    for(int i = 0 ; i < times ; ++ i)
    {
        DBHandle *handle = manager->new_connection();
        
    }
}
