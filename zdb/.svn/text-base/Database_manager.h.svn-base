/*
 * =====================================================================================
 *
 *       Filename:  Database_manager.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/13 20:46:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <list>

#include <zdb/zdb.h>

/* standard url is formatted like this :
 * database://[user:password@][host][:port]/database
 * [?propertyName1][=propertyValue1][&propertyName2][=propertyValue2]...
 * */

#define MAX_CONNECTION_NUMBER 20

class DBHandle;
class DBManager
{
    public :
        /* construction function , 
         * without parameter , can use it with singleton
         */
        DBManager();
        /* destruction function 
         * free all resources  
         */
        ~DBManager();

        /* initialize and start connection pool
         * para1 a standard url string such as db://user:passwd@ip:port/db_name?option
         * para2 max connections number of connection pool
         * return 0 for success or -1 for failed.
         */
        int initialize(const std::string & , int = MAX_CONNECTION_NUMBER);

        /* get a connection from connection pool 
         * return NULL for reach max connection or DBHandle pointer
         */
        DBHandle *new_connection();

        /* get initialize url
         * return the url string
         */
        std::string get_url()
        {
            return URL_toString(m_purl);
        }

        /*  
        ConnectionPool_T get_pool()
        {
            return m_ppool;
        }
        */

        int get_active_connections()
        {
            return ConnectionPool_active(m_ppool);
        }

        void set_max_connections(int value)
        {
            ConnectionPool_setMaxConnections(m_ppool , value);
        }

        int get_max_connections()
        {
            return ConnectionPool_getMaxConnections(m_ppool);
        }

    private :
        ConnectionPool_T m_ppool;
        URL_T            m_purl;
        bool             m_binit;
};
