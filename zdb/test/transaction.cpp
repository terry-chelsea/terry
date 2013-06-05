/*
 * =====================================================================================
 *
 *       Filename:  transaction.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 20:20:14
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

int main()
{
    DBManager *manager = new DBManager();
    if(!manager || manager->initialize(url_line) < 0)
    {
        cerr<<"manager initialize error !"<<endl;
        return -1;
    }

    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        cerr<<"create connection error !"<<endl;
        return -1;
    }

    for(int i = 0 ; i < 100 ; ++ i)
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
        
        if(handle->execuate("insert into yu values (%d , \"%s\" , %d)" , 0 , name[0].c_str() , num1) < 0 || handle->execuate("insert into yu values (%d , \"%s\" , %d)" , 0 , name[1].c_str() , num2) < 0)
        {
            cerr<<"execuate error  "<<endl;
        }
        if((i + 1)% 3 == 0)
        {
            cout<<"rollback index : "<<num1<<" and "<<num2<<endl;
            if(handle->rollback_transaction() < 0)
            {
                cerr<<"rollback error !"<<endl;
                break;
            }
        }
        else
        {
            cout<<"commit index : "<<num1<<" and "<<num2<<endl;
            if(handle->commit_transaction() < 0)
            {
                cerr<<"commit error !"<<endl;
                break;
            }
        }
    }

    return 0;
}
