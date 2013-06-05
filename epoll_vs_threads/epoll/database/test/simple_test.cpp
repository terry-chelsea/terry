/*
 * =====================================================================================
 *
 *       Filename:  simple_test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 16:31:10
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
#include <cstring>
#include <cstdlib>
#include <string>
#include <sys/time.h>

using namespace std;

const char *url_line = "mysql:/""/root:mysql@localhost:3306/ca";

const char *insert_sql = "insert into new_client values";
#define MAX_LINE 32
const char *characters = "1234567890abcdefghijklmnopqrstuvexyz";
const int CHAR_NUM = 36;
#define NAME_MAX_LEN 16

void get_random_name(std::string &name)
{
    name.clear();
    for(int i = 0 ; i < NAME_MAX_LEN ; ++ i)
        name += characters[rand() % CHAR_NUM];
}

int get_random_text(char *text)
{
    int len = rand() % MAX_LINE;
    for(int i = 0 ; i < len ; ++ i)
    {
        text[i] = characters[rand() % CHAR_NUM] ;
    }
   
    text[len] = '\0';
    return len;
}

int main(int argc , char *argv[])
{
    if(argc != 2)
    {
        cerr<<"./Usage : ./test counters"<<endl;
        return -1;
    }

    srand(time(NULL));
    int times = atoi(argv[1]);
    char addtion[MAX_LINE];

    DBManager *manager = new DBManager();
    if(manager->initialize(url_line) < 0)
    {
        cerr<<"database manager initialize error !"<<endl;
        return -1;
    }

    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        cerr<<"create new connection error !"<<endl;
        return -1;
    }
    DBPreStatementHandle *pre = handle->prepare_statement("%s (? , ? , ? , ?)" , insert_sql);
    if(NULL == pre)
    {
        cerr<<"create new prepare statement error !"<<endl;
        return -1;
    }

    if(handle->execuate("delete from new_client where login_times != 1") < 0)
    {
        cerr<<"execute error !"<<endl;
        return -1;
    }

    double start = 1.0123;
    for(int i = 0 ; i < times ; ++ i)
    {
        std::string name ;
        get_random_name(name);
        start += 0.0146;
        long long int money = rand() % 10000000;
        
        char text[MAX_LINE];
        int len = get_random_text(text);

//        if(handle->execuate("%s (%d , \"%s\" , %lf , %lld , \"%s\")" , insert_sql , 0 , name.c_str() , start , money , text) < 0)
/*
        pre->set_int_parameter(1 , 0);
        pre->set_string_parameter(2 , name);
        pre->set_long_parameter(4 , money);
        pre->set_double_parameter(3 , start);
        pre->set_blob_parameter(5 , (void *)text , len);
        */
        pre->set_string_parameter(1 , "fengyu");
        pre->set_string_parameter(2 , "12344565");
        pre->set_string_parameter(3 , "1990-1-1 12:3:0");
        pre->set_int_parameter(4 , 0);
        if(pre->execute_statement() < 0)
        {
            cerr<<"execute error !"<<endl;
            break;
        }
    }

    handle->finish_prepare(pre);
    delete handle;
    handle = NULL;
    delete manager;
    manager = NULL;

    return 0;
}

