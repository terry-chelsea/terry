/*
 * =====================================================================================
 *
 *       Filename:  insert_to_yu.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 18:07:16
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
    if(manager->initialize(url_line) < 0)
    {
        cerr<<"initialize error !"<<endl;
        return -1;
    }

    DBHandle *han = manager->new_connection();
    if(han == NULL)
    {
        cerr<<"create connection error !"<<endl;
        return -1;
    }

    DBPreStatementHandle *pre = han->prepare_statement("insert into yu values(0 , ? , ?)");
    if(pre == NULL)
    {
        cerr<<"create prepare statement error !"<<endl;
        return -1;
    }

    for(int i = 0 ; i < 100 ; ++ i)
    {
        std::string name;
        get_name(name);
        if(pre->set_string_parameter(1 , name) < 0 || 
                pre->set_int_parameter(2 , rand() % 1000) < 0)
        {
            cerr<<"set parameter error !"<<endl;
            break;
        }

        if(pre->execute_statement() < 0)
        {
            cerr<<"prepare statement execute error !"<<endl;
            break;
        }
    }

    han->finish_prepare(pre);
    delete han;
    delete manager;

    return 0;
}
