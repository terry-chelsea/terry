/*
 * =====================================================================================
 *
 *       Filename:  database.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/26/13 18:55:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_DATABASE_H_
#define _H_DATABASE_H_

#include <string>
#include <vector>

using namespace std;

struct RET_INFO
{
    string token ;
    string pwd;
    long long uid;
};

using namespace std;

#define GET_ALL_USER 0
#define GET_ALL_ACTIVE_USER 1

extern unsigned int get_time_from_string(const string &buf);

class DBManager;
class Database_Info
{
    public : 
        Database_Info(string url);

        ~Database_Info();

        struct RET_INFO *deal_with_login(const std::string &name , int &ret);
        
        int create_user_table() ;

    private : 
        DBManager *database_manager;
};

#endif
