/*
 * =====================================================================================
 *
 *       Filename:  prepare_handle.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/12/13 11:55:02
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

class DBResultHandle ;
class DBHandle;
class DBPreStatementHandle
{
    public :
        DBPreStatementHandle(DBHandle * , PreparedStatement_T , std::string );
        ~DBPreStatementHandle();

        int set_string_parameter(int , std::string );
        int set_int_parameter(int , int);
        int set_long_parameter(int , long long int);
        int set_double_parameter(int , double);
        int set_blob_parameter(int , const void * , int);

        int execute_statement();

        int execute_query_statement();
        
        std::string get_prepare_string()
        {
            return m_sprepare_sql;
        }

        /* -------------- get result functions ------------- */
        /* get next result , mive the cursor to next postion
         * NOTE:the result is initially location before the first rows , so you should
         * use this function before use result
         * return false when no more rows and true when new row is valid
         */
        bool next_result();

        /* get_***_result : get result of specific type
         * para1 is index of this cloumn
         * para2 and para3(blob) is pass bt reference to get result
         * return -1 means error and 0 means success
         */
        int get_int_result(int , int &);
        int get_string_result(int , std::string &);
        int get_double_result(int , double &);
        int get_long_result(int , long long int &);
        int get_blob_result(int , const void *& , int &);

        /* get cloumn count of last result */
        int get_cloumn_count();
        /* get cloumn name of specific index */
        int get_cloumn_name(int , std::string &);
        /* return the size of content in specific cloumn of current row */
        long get_cloumn_size(int);

    private :
        PreparedStatement_T m_ppre;
        DBResultHandle *m_presult;
        std::string m_sprepare_sql;
        DBHandle *m_pconn;
};
