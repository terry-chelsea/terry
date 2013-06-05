#ifndef _H_LOGIN_TASK_H_
#define _H_LOGIN_TASK_H_

#include "Task.h"
#include <string>

using namespace std;

struct Login_info
{
    std::string user_name;
};

#define DATABASE_TABLE_NAME "new_client"
struct RET_INFO
{
    string token ;
    string pwd;
    long long uid;
};

class WorkItem;
class Token;
class LoginTask : public Task 
{
    public : 
        LoginTask() : 
            Task() ,
            m_item(NULL) 
        {

        }

        ~LoginTask();

        int init(struct TPara para , Callback cb);
        void login_back(int ret);
        int check_user(void *para);

    private :
        void make_response_string();
    
    private :
        WorkItem *m_item;
        struct Login_info m_info;
        struct RET_INFO *m_ret_info;
};

#endif
