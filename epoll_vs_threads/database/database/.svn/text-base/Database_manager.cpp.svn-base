/*
 * =====================================================================================
 *
 *       Filename:  Database_manager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/11/13 23:35:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Database_manager.h"
#include "Database_handle.h"

#define LOG_INSIDE_ERROR (std::cerr)
#define LOG_INSIDE_INFO  (std::cout)
#define LOG_END          (std::endl)

DBManager::DBManager()
    :m_ppool(NULL) , 
    m_purl(NULL) , 
    m_binit(false)
{
}

DBManager::~DBManager()
{
    if(m_binit)
    {
        ConnectionPool_free(&m_ppool);
        m_ppool = NULL;

        URL_free(&m_purl);
        m_purl = NULL;
        m_binit = false;
    }
}

int DBManager::initialize(const std::string & url_str , int max_conn)
{
    URL_T url = URL_new(url_str.c_str());
    if(NULL == url)
    {
        LOG_INSIDE_ERROR<<"DBManager::Create new URL handle error , error URL : "
            <<url_str<<LOG_END;
        return -1;
    }

    ConnectionPool_T pool = ConnectionPool_new(url);
    if(NULL == pool)
    {
        LOG_INSIDE_ERROR<<"DBManager::Create new connection pool error !"<<LOG_END;
        URL_free(&url);
        return -1;
    }

    m_ppool = pool;
    m_purl = url;
    m_binit = true;
    
    return 0;
}

DBHandle *DBManager::new_connection()
{
    Connection_T conn = ConnectionPool_getConnection(m_ppool);
    if(NULL == conn)
    {
        LOG_INSIDE_ERROR<<"DBManager::No connection left in connection pool! you can set max connections or retry later..."<<LOG_END;
        return NULL;
    }

    DBHandle *handle = new DBHandle(this , conn);
    if(NULL == handle)
    {
        LOG_INSIDE_ERROR<<"DBManager::create DBHandle error !"<<LOG_END;
        ConnectionPool_returnConnection(m_ppool , conn);
        return NULL;
    }

//    LOG_INSIDE_INFO<<"Create new connection handle..."<<LOG_END;
    return handle;
}

