/*
 * =====================================================================================
 *
 *       Filename:  mutil_transaction.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 20:41:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "../database.h"
#include <iostream>
#include <cstdlib>

using namespace std;

const char *url_line = "mysql:/""/root:mysql@localhost:3306/terry";

const char *first_name = "abcdefg";
const char *second_name = "hijklmn";
const char *third_name = "opqrst ";
const char *forth_name = "uvwxyz ";
#define LENGTH 7

void get_name(std::string &name)
{
    name.clear();
    name += first_name[rand() % LENGTH];
    name += second_name[rand() % LENGTH];
    name += third_name[rand() % LENGTH];
    name += forth_name[rand() % LENGTH];
}

DBManager *manager = NULL;

void* func(void *args)
{
    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        cerr<<"create connection error !"<<endl;
        return NULL;
    }

    for(int i = 0 ; i < 1000 ; ++ i)
    {
        std::string name[2];
        get_name(name[0]);
        get_name(name[1]);
        int num1 = rand() % 1000;
        int num2 = rand() % 1000;

        if(handle->begin_transaction() < 0)
        {
            cerr<<"begin transaction error !"<<endl;
            break;
        }
        
        bool ret = (handle->execuate("insert into yu values (%d , \"%s\" , %d)" , 0 , name[0].c_str() , num1) < 0) || (handle->execuate("insert into yu values (%d , \"%s\" , %d)" , 0 , name[1].c_str() , num2) < 0);
        if(ret)
        {
            cerr<<"execuate error  "<<endl;
        }
        if(ret)
        {
            cerr<<"roll back in thread "<<pthread_self()<<endl;
            if(handle->rollback_transaction() < 0)
            {
                cerr<<"rollback error !"<<endl;
                break;
            }
        }
        else
        {
            if(handle->commit_transaction() < 0)
            {
                cerr<<"commit error !"<<endl;
                break;
            }
        }
    }

    return NULL;
}

int main(int argc , char *argv[])
{
    if(argc != 2)
    {
        cerr<<"Uasge : ./test threads\n";
        return -1;
    }

    manager = new DBManager();
    if(!manager || manager->initialize(url_line) < 0)
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
        if(pthread_create(tid + i , NULL , func , NULL) < 0)
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

