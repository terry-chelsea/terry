#include "Login_task.h"
#include "protocol.h"
#include "ThreadPool.h"
#include "Dispatcher.h"
#include "debug.h"
#include <tr1/functional>
#include <cstdio>

#include "database/database.h"


LoginTask::~LoginTask()
{
    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }
    if(m_ret_info != NULL)
    {
        delete m_ret_info;
        m_ret_info = NULL;
    }
}

//#define ON_DATABASE

int LoginTask::init(struct TPara para , Callback cb)
{
    Task::init(para , cb);
    CA::pb_MSG_SYS_CA_USER_LOGIN login_info;
    std::string in_data(para.load , para.header.length);
    Task::init_load_buf(MSG_SYS_CA_USER_LOGIN_ACK);

    if((!login_info.ParseFromString(in_data)) || (!login_info.IsInitialized()))
    {
        LOG_ERROR("Parse protobuf error in login task !");
        return PROTOBUF_ERROR;
    }

#ifdef ON_DATABASE
    m_ret_info = new struct RET_INFO;
    m_ret_info->token = "fengxiaoyu";
    m_ret_info->pwd = "ASGCNDCJSDCJNNJFVNJDSVDJNVDSJVJND";
    m_ret_info->uid = 10016;

    Task::m_ret = CA_WORKS_OK;
    make_response_string();
    Task::do_callback();
    return 0;

#endif

    string name = login_info.name();

    m_info.user_name = name;

    m_item = new WorkItem();
    if(NULL == m_item)
    {
        Task::m_ret = CA_INSIDE_ERROR;
        LOG_ERROR("Create workitem in login task error !");
        return -1;
    }

    m_item->init(std::tr1::bind(&LoginTask::login_back , this , 
                std::tr1::placeholders::_1) , std::tr1::bind(&LoginTask::check_user , 
                    this , std::tr1::placeholders::_1) , &m_info);

    Dispatcher::getInstancePtr()->send_request(m_item);
    return 0;
}

void LoginTask::make_response_string()
{
    if(NULL == m_ret_info)
        return ;

    std::string out_data;
    CA::pb_MSG_SYS_CA_USER_LOGIN_ACK ack_info;
    ack_info.set_token(m_ret_info->token);
    ack_info.set_password(m_ret_info->pwd);
    ack_info.set_user_id(m_ret_info->uid);

    if((!ack_info.IsInitialized()) || (!ack_info.SerializeToString(&out_data)))
    {
        Task::m_ret = PROTOBUF_ERROR;
        LOG_ERROR("Parse out info to string error !");
        return ;
    }

    int sz = out_data.size();
    char *out = new char[sz];
    memcpy(out , out_data.c_str() , sz);
    m_para.load = out;
    m_para.header.length = sz;

    delete m_ret_info;
    m_ret_info = NULL;
}

//for 0 success , user info is in m_result , 1 for user not exist , 2 means password is errror , and -1 for error...
void LoginTask::login_back(int ret)
{
    if(-1 == ret)
    {
        Task::m_ret = CA_INSIDE_ERROR;
        LOG_ERROR("Login in workitem execute failed , name is %s" , 
                m_info.user_name.c_str());
        goto RET;
    }
    else if(1 == ret)
    {
        Task::m_ret = USER_NAME_NOT_EXIST;
        LOG_INFO("Login error : user name %s is not exist !" , m_info.user_name.c_str());
        goto RET;
    }
    
    make_response_string();

RET :
    Task::do_callback();
    return ;
}

#define SQL_LENGTH 256
#define LOGIN_SQL "select %s , %s , %s from %s where %s= \'%s\' "
#define TABLE_ID_NAME  "id"
#define TABLE_NAME_NAME "name"
#define TABLE_PASSWORD_ANAME "password"
#define TIME_NAME  "login_times"

#include <sys/time.h>
extern DBManager *database_manager;

//#define SNPRINTF
int LoginTask::check_user(void *para)
{
    WorkItem *item = (WorkItem *)para;
    void *para_info = item->get_exec_result();
    struct Login_info *info = (struct Login_info *)(para_info);
    std::string name = info->user_name;
    item->set_exec_result(NULL);

    DBManager *manager = database_manager;
    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        LOG_ERROR("create new database connection error !");
        return -1;
    }

#ifdef _COUNT_
    struct timeval start , end;
    double gap = 0;
    gettimeofday(&start , NULL);
#endif

    if(handle->begin_transaction() < 0)
    {
        LOG_ERROR("Start a new stransaction error !");
        return -1;
    }

    struct RET_INFO *ret_info = NULL;
    long long int uid = 0;
    std::string db_password;
    std::string db_name;
    int ret = 0;
    std::string sqll;
#ifdef SNPRINTF
    char sql[SQL_LENGTH];
    int len = snprintf(sql , SQL_LENGTH , LOGIN_SQL  ,  
            TABLE_ID_NAME , TABLE_NAME_NAME , TABLE_PASSWORD_ANAME ,  
            DATABASE_TABLE_NAME , TABLE_NAME_NAME , name.c_str());
    sql[len - 1] = '\0';
    sqll.assign(sql);
#else
    sqll = "select id , name , password from "DATABASE_TABLE_NAME" where name = \'"
        +name + "\'";
#endif

    LOG_INFO("execute query sql : \"%s\"" , sqll.c_str());

    if(handle->execute_query(sqll.c_str()) < 0)
    {
        LOG_ERROR("Execute query sql : %s error !" , sqll.c_str());
        ret = -1;
        handle->rollback_transaction();
        goto FREE;
    }

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
#ifdef SNPRINTF
    memset(sql , 0 , SQL_LENGTH);
    len = snprintf(sql , SQL_LENGTH , "update %s set %s = %s +1 where %s=\'%s\' " , 
            DATABASE_TABLE_NAME , TIME_NAME , TIME_NAME , TABLE_NAME_NAME , name.c_str());
    sql[len - 1] = '\0';
    sqll.assign(sql);
#else
    sqll = "update "DATABASE_TABLE_NAME" set login_times = login_times +1 where name=\'"
        +name + "\'";
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

    handle->commit_transaction();
#ifdef _COUNT_
    gettimeofday(&end , NULL);
    gap = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    std::cerr<<"Time gap : "<<gap / 1000<<"ms"<<std::endl;
#endif

    ret_info = new struct RET_INFO;
    ret_info->token = db_name;
    ret_info->pwd = db_password;
    ret_info->uid = uid;

    m_ret_info = ret_info;
    ret = 0;

FREE : 
    delete handle;
    handle = NULL;

    return ret;
}
