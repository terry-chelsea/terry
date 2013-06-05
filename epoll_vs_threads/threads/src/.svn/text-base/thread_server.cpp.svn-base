
#include "work_func.h"
#include "Header.h"
#include "database.h"
#include "log.h"
#include "ca_login.pb.h"
#include "protocol.h"
#include "operations.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <time.h>
#include <signal.h>

const char *url_line = "mysql:/""/root:mysql@localhost:3306/ca";
Database_Info *database_info = NULL;

static Work_Func_map function_map;
void register_all_functions(Work_Func_map &function);

void get_data(char *buf , int len);
int ca_client(int fd);
int deal_with_req(const struct Client_Req &req);

void sig_int(int signo)
{
    if(database_info != NULL)
    {
        delete database_info;
        database_info = NULL;
    }

    exit(0);
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
    unsigned  short port = atoi(argv[1]);

    register_all_functions(function_map);

    //this will never deleted,,,
    database_info = new Database_Info(url_line);

    if(database_info->create_user_table() < 0)
    {
        LOG_ERROR("Create user infomation table error !");
        return -1;
    }

    start_mutil_thread_server(NULL , port , ca_client);

    FINISH_DEBUG();

    if(database_info != NULL)
    {
        delete database_info;
        database_info = NULL;
    }

    return 0;
}

void get_data(char *buf , int len)
{
    time_t now = time(NULL);
    struct tm time_buf;
    if(gmtime_r(&now , &time_buf) == NULL)
        LOG_SYSCALL_EXIT("Get time of now error");

    int length = snprintf(buf , len , "%d_%d_%d" , time_buf.tm_year + 1900 , time_buf.tm_mon , time_buf.tm_mday);
    buf[length] = '\0';
}

int ca_client(int fd)
{
    char *peer_ip = NULL;

    bool error_flag = false;
    bool close_flag = false;
    struct Client_Req req;
    while(1)
    {
        memset(&req , 0 , sizeof(req));
        Header head;
        memset(&head , 0 , sizeof(head));
        int ret = readn_blocking(fd , (char *)&head , sizeof(head));
        if(ret < 0)
        {
            error_flag = true;
            break;
        }
        else if(0 == ret)
        {
            close_flag = true;
            break;
        }

        req.fd = fd;
        memcpy(&(req.head) , &head , sizeof(head));
        int len = head.length;
        char *load = NULL;
        if(len > 0)
        {
            load = (char *)malloc(len);
            memset(load , 0 , len);
            ret = readn_blocking(fd , load , len);
            if(ret < 0)
            {
                error_flag = true;
                free(load);
                break;
            }
            else if(0 == ret)
            {
                close_flag = true;
                free(load);
                break;
            }
        }
        req.load = load;
        if(deal_with_req(req) < 0)
        {
            LOG_ERROR("deal with client request error");
            error_flag = true;
            break;
        }

        if(load != NULL)
        {
            free(load);
            load = NULL;
        }
    }

    if(peer_ip != NULL)
    {
        free(peer_ip);
        peer_ip = NULL;
    }
    if(error_flag)
    {
        LOG_ERROR("read data blocking error from ip %s" ,
                ((peer_ip == NULL) ? "unknow" : peer_ip));
        close(fd);
        return -1;
    }
    else if(close_flag)
    {
        LOG_INFO("Client close this connection from ip %s" , 
                ((peer_ip == NULL) ? "unknow" : peer_ip));
        close(fd);
        return 0;
    }

    return 0;
}

int deal_with_req(const struct Client_Req &req)
{
    unsigned int cmd = req.head.cmd;
    char *info = NULL;
    work_func func = get_function(function_map , cmd , info);
    if(NULL == func)
    {
        LOG_WARNING("Undefined command type %d" , cmd);
        return -1;
    }
    if(func(req) < 0)
    {
        LOG_ERROR("%s error" , info);
        return -1;
    }

    return 0;
}


void register_all_functions(Work_Func_map &function)
{
    register_function(function , MSG_SYS_CA_USER_LOGIN , user_login , "user login");
}

