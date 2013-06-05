
#include "work_func.h"
#include "Header.h"
#include "database.h"
#include "operations.h"
#include "ca_login.pb.h"
#include "protocol.h"
#include "log.h"
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>

extern Database_Info *database_info;

using namespace std;

int send_data(int fd , int cmd , int err , int para1 , int para2 , const string &dat)
{
    bool emp = dat.empty();
    Header head;
    memset(&head , 0 , sizeof(head));
    head.cmd = cmd;
    head.error = err;
    head.para1 = para1;
    head.para2 = para2;
    if(emp)
        head.length = 0;
    else
        head.length = dat.size();

    int out_length = head.length + sizeof(head);
    char *out_data = new char[out_length];
    memcpy(out_data , &head , sizeof(head));
    if(!emp)
        memcpy(out_data + sizeof(head) , dat.c_str() , head.length);

    int ret = writen_blocking(fd , out_data , out_length);
    delete []out_data;
    out_data = NULL;

    return ret;
}

//#define NO_DATABASE

int user_login(const struct Client_Req &req)
{
    std::string out_data;
    struct RET_INFO *ret_info = NULL;
    int ret =  0;
    string name;
    uint32_t err_code = CA_WORKS_OK;
    CA::pb_MSG_SYS_CA_USER_LOGIN login_info;
    std::string in_data(req.load , req.head.length);
    CA::pb_MSG_SYS_CA_USER_LOGIN_ACK ack_info;

    if((!login_info.ParseFromString(in_data)) || (!login_info.IsInitialized()))
    {
        LOG_ERROR("Parse protobuf error in login task !");
        goto RET;
    }

    name.assign(login_info.name());

#ifdef NO_DATABASE
    ret_info = new RET_INFO;
    ret_info->token = "fengxiaoyu";
    ret_info->pwd = "ASGCNDCJSDCJNNJFVNJDSVDJNVDSJVJND";
    ret_info->uid = 10016;
    err_code = CA_WORKS_OK;
#else
    
    ret_info = database_info->deal_with_login(name , ret);

    if(-1 == ret)
    {
        err_code = CA_INSIDE_ERROR;
        LOG_ERROR("Login in workitem execute failed , name is %s" , 
                name.c_str());

        goto RET;
    }
    else if(1 == ret)
    {
        err_code = USER_NAME_NOT_EXIST;
        LOG_INFO("Login error : user name %s is not exist !" , name.c_str());
        goto RET;
    }
    
    if(NULL == ret_info)
    {
        err_code = CA_INSIDE_ERROR;
        LOG_WARNING("CA inside error !");
        goto RET;
    }
#endif

    ack_info.set_token(ret_info->token);
    ack_info.set_password(ret_info->pwd);
    ack_info.set_user_id(ret_info->uid);

    if((!ack_info.IsInitialized()) || (!ack_info.SerializeToString(&out_data)))
    {
        err_code = PROTOBUF_ERROR;
        LOG_ERROR("Parse out info to string error !");
        goto RET;
    }

RET :
    if(ret_info != NULL)
    {
        delete ret_info;
        ret_info = NULL;
    }

    if(send_data(req.fd , MSG_SYS_CA_USER_LOGIN_ACK , err_code , req.head.para1 , req.head.para2 , out_data) < 0)
    {
        LOG_ERROR("Write create user ack error !");
        close(req.fd);
        return -1;
    }
    return 0;
}

