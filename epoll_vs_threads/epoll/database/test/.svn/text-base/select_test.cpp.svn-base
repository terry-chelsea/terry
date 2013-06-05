/*
 * =====================================================================================
 *
 *       Filename:  select.test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 17:33:01
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

using namespace std;

const char *url_line = "mysql:/""/root:mysql@localhost:3306/terry";

void print_text(unsigned char *text , int len)
{
    for(int i = 0 ; i < len ; ++ i)
        cerr<<text[i];
}

int main()
{
    DBManager *manager = new DBManager();
    if(manager->initialize(url_line) < 0)
    {
        cerr<<"database manager intialize error !"<<endl;
        return -1;
    }
    
    DBHandle *handle = manager->new_connection();
    if(NULL == handle)
    {
        cerr<<"create new connection error !"<<endl;
        return -1;
    }

    if(handle->execute_query("select * from test") < 0)
    {
        cerr<<"select all thing error !"<<endl;
        return -1;
    }

    int id = 0;
    double rate = 0.0;
    std::string str;
    long long int llmonery;
    const void *text;
    int text_len;
    while(handle->next_result())
    {
        if((handle->get_int_result(1 , id) < 0) || 
                (handle->get_string_result(2 , str) < 0) || 
                (handle->get_double_result(3 , rate) < 0) || 
                (handle->get_long_result(4 , llmonery) < 0) || 
                (handle->get_blob_result(5 , text , text_len) < 0))
        {
            cerr<<"some one is error !"<<endl;
            return -1;
        }
    
        unsigned char *text_ch = new unsigned char[text_len + 1];
        memcpy(text_ch , text , text_len);

        cerr<<"id: "<<id<<" name: "<<str<<" rate: "<<rate<<" monery: "<<llmonery<<" text: ";
        print_text(text_ch , text_len);
        cerr<<endl;

        delete [] text_ch;
    }

    return 0;
}
