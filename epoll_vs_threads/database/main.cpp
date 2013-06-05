
#include "database/database.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/time.h>

using namespace std;
//#define GLOBAL_HANDLE
//#define TRANSACTION

DBManager *DBmanager = NULL;
const char *url_line = "mysql:/""/root:mysql@localhost:3306/ca";
#ifdef GLOBAL_HANDLE
DBHandle *global_handle = NULL;
#endif

int works(string name);

int main()
{
    DBmanager = new DBManager();
    if(DBmanager->initialize(url_line) < 0)
    {
        cerr<<"database manager initialize error !"<<endl;
        return -1;
    }

    DBHandle *handle = DBmanager->new_connection();
    if(handle == NULL)
    {
        cerr<<"allocate new connection error !"<<endl;
        return -1;
    }
    
    string sql("select name from new_client where id < 2000");
    if(handle->execute_query(sql.c_str()) < 0)
    {
        cerr<<"execute query sql : "<<sql<<endl;
        return -1;
    }

    vector<string> all_names;
    while(handle->next_result())
    {
        string name;
        if(handle->get_string_result(1 , name) < 0)
        {
            cerr<<"get result error !"<<endl;
            return -1;
        }

        all_names.push_back(name);
    }

    delete handle;
    handle = NULL;

    int sz = all_names.size();
    cerr<<"All names has "<<sz<<" items"<<endl;
#ifdef GLOBAL_HANDLE
    global_handle = DBmanager->new_connection();
    if(NULL == global_handle)
    {
        cerr<<"allocate global handle error !"<<endl;
        return -1;
    }
#endif
    
    struct timeval start , end;
    gettimeofday(&start , NULL);
    for(int i = 0 ; i < sz ; ++ i)
    {
        string name = all_names[i];
        if(works(name) < 0)
        {
            cerr<<"works error !"<<endl;
            return -1;
        }
    }
    gettimeofday(&end , NULL);

    double gap = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    double all_times = gap / 1000;
    cerr<<"All used time : "<<all_times<<" ms"<<endl;
    cerr<<"Means operations used "<<all_times / sz<<" ms"<<endl;

    return 0;
}

int works(string name)
{
#ifndef GLOBAL_HANDLE
    DBManager *manager = DBmanager;
    DBHandle *handle = manager->new_connection();

#else
    DBHandle *handle = global_handle;
#endif
    if(NULL == handle)
    {
        cerr<<"create new database connection error !"<<endl;
        return -1;
    }

#ifdef TRANSACTION
    if(handle->begin_transaction() < 0)
    {
        cerr<<"Start a new stransaction error !"<<endl;
        return -1;
    }
#endif

    int ret = 0;
    long long int uid = 0;
    std::string db_password;
    std::string db_name;

    string sql = "select id , name , password from new_client where name = \'" 
        + name + "\'";
    if(handle->execute_query(sql.c_str()) < 0)
    {
        ret = -1;
        cerr<<"Execute query sql : "<<sql<<" error !"<<endl;
#ifdef TRANSACTION
        handle->rollback_transaction();
#endif
        goto FREE;
    }

    if(!handle->next_result())
    {
        cerr<<"User name "<<name.c_str()<<" did not exist in database !"<<endl;
        ret = 1;
        goto FREE;
    }

    if((handle->get_long_result(1 , uid) < 0) || 
            (handle->get_string_result(2 , db_name) < 0) || 
            (handle->get_string_result(3 , db_password) < 0) )
    {
        cerr<<"Get result from query result error !"<<endl;
        ret = -1;
        goto FREE;
    }

    sql = "update memory_client set login_times = login_times +1 where name=\'" 
        + name + "\'" ;
    if(handle->execuate(sql.c_str()) < 0)
    {
        cerr<<"Execute uodate sql : "<<sql<<" error !"<<endl;
        ret = -1;
#ifdef TRANSACTION
        handle->rollback_transaction();
#endif
        goto FREE;
    }

#ifdef TRANSACTION
    handle->commit_transaction();
#endif
    ret = 0;

FREE : 

#ifndef GLOBAL_HANDLE
    delete handle;
    handle = NULL;
#endif

    return ret;
}
