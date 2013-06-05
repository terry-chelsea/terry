/*
 * =====================================================================================
 *
 *       Filename:  result_handle.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/12/13 12:02:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include <iostream>
#include <string>
#include <cstdlib>

#include <zdb/zdb.h>

class DBResultHandle
{
    public :
       DBResultHandle(ResultSet_T);
       ~DBResultHandle();

       int get_int_result(int , int &);
       int get_string_result(int , std::string &);
       int get_long_result(int , long long int &);
       int get_double_result(int , double &);
       int get_blob_result(int , const void *& , int &);

       int get_cloumn_count();
       int get_cloumn_name(int , std::string &);
       long get_cloumn_size(int);

       bool next_result();

    private :
        ResultSet_T m_pres;
        int m_iColunm;
};
