/*
 * =====================================================================================
 *
 *       Filename:  Database_handle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/13/13 20:15:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Database_handle.h"
#include "Database_manager.h"
#include "prepare_handle.h"
#include "result_handle.h"
#include <cstdarg>           //for va_list and vsnprintf
#include <cstdio>

#define LOG_ERR_INSIDE  (std::cerr)
#define LOG_WARNING_INSIDE (std::cerr)
#define LOG_INFO_INSIDE (std::cout)
#define LOG_END         (std::endl)

#define MAX_SQL_LINE  1024

DBHandle::DBHandle(DBManager * manager , Connection_T conn)
    :m_pconn(conn) , 
    m_lpre() , 
    m_presult(NULL) , 
    m_pmanager(manager)
{
}

DBHandle::~DBHandle()
{
    this->clear_connection();
//    Connection_close(m_pconn);

//    std::cerr<<"destory connection : "<<pthread_self()<<" Current active connections "<<m_pmanager->get_active_connections()<<std::endl;
    
    m_pmanager->signal_waiting_thread(m_pconn);
    m_pconn = NULL;
}

void DBHandle::clear_connection()
{
    DBPreStatementHandle *pre = NULL;;
    std::list<DBPreStatementHandle *>::iterator it = m_lpre.begin();
    for( ; it != m_lpre.end() ; ++ it)
    {
        pre = *it;
        delete pre;
    }
    m_lpre.clear();

    if(NULL != m_presult)
    {
        delete m_presult;
        m_presult = NULL;
    }
}

int DBHandle::begin_transaction()
{
    TRY
    {
        Connection_beginTransaction(m_pconn);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"DBHandle::begin transaction error !"<<LOG_END;
        return -1;
    }
    END_TRY;
    
//    LOG_INFO_INSIDE<<"Begin transaction..."<<LOG_END;
    return 0;
}

int DBHandle::commit_transaction()
{
    TRY
    {
        Connection_commit(m_pconn);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"DBHandle::commit last transaction error !"<<LOG_END;
        return -1;
    }
    END_TRY;

//    LOG_INFO_INSIDE<<"Commit last transaction..."<<LOG_END;
    return 0;
}

int DBHandle::rollback_transaction()
{
    TRY
    {
        Connection_rollback(m_pconn);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"DBHandle::rollback last transaction error !"<<LOG_END;
        return -1;
    }
    END_TRY;

//    LOG_INFO_INSIDE<<"Rollback last transaction..."<<LOG_END;
    return 0;
}

int DBHandle::last_insert_rowId()
{
    return Connection_lastRowId(m_pconn);
}

int DBHandle::rows_changes()
{
    return Connection_rowsChanged(m_pconn);
}

int DBHandle::execuate(const char *fmt , ...)
{
    va_list var;
    va_start(var , fmt);
    char sql[MAX_SQL_LINE];
    memset(sql , 0 , MAX_SQL_LINE);

    if(NULL != m_presult)
    {
        delete m_presult;
        m_presult = NULL;           //clear result every execute
    }
    vsnprintf(sql , MAX_SQL_LINE , fmt , var);
    TRY
    {
        Connection_execute(m_pconn , sql);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Execute SQL statement : "<<sql<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    END_TRY;

    return 0;
}

int DBHandle::execute_query(const char *fmt , ...)
{
    va_list var;
    va_start(var , fmt);
    char sql[MAX_SQL_LINE];
    memset(sql , 0 , MAX_SQL_LINE);

    if(m_presult != NULL)
    {
        delete m_presult;
        m_presult = NULL;
    }
    vsnprintf(sql , MAX_SQL_LINE , fmt , var);
    ResultSet_T ret = NULL;
    TRY
    {
        ret = Connection_executeQuery(m_pconn , sql);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Execute query SQL statement : "<<sql<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    END_TRY;

    DBResultHandle *res_handle = new DBResultHandle(ret);
    m_presult = res_handle;
    return 0;
}

DBPreStatementHandle *DBHandle::prepare_statement(const char * fmt, ...)
{
    va_list var;
    va_start(var , fmt);
    char pre_sql[MAX_SQL_LINE];
    memset(pre_sql , 0 , MAX_SQL_LINE);

    vsnprintf(pre_sql , MAX_SQL_LINE , fmt , var);
    PreparedStatement_T pre = NULL;
    TRY
    {
        pre = Connection_prepareStatement(m_pconn , pre_sql);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Create new prepare statement : "<<pre_sql<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return NULL;
    }
    END_TRY;

    DBPreStatementHandle *pre_handle = new DBPreStatementHandle(this , pre , pre_sql);
    m_lpre.push_back(pre_handle);
    return pre_handle;
}

std::string DBHandle::last_error()
{
    return Connection_getLastError(m_pconn);
}

#define MAX_QUERY_TIMES  3000

void DBHandle::set_query_timeout(int ms)
{
    if(ms > MAX_QUERY_TIMES)
    {
        LOG_ERR_INSIDE<<"DBHandle::query timeout should less then "<<MAX_QUERY_TIMES<<" millseconds.."<<LOG_END;
        ms = MAX_QUERY_TIMES;
    }

    Connection_setQueryTimeout(m_pconn , ms);
}

int DBHandle::get_query_timeout()
{
    return Connection_getQueryTimeout(m_pconn);
}

void DBHandle::set_max_rows(int rows)
{
    Connection_setMaxRows(m_pconn , rows);
}

int DBHandle::get_max_rows()
{
    return Connection_getMaxRows(m_pconn);
}

bool DBHandle::next_result()
{
    if(m_presult != NULL)
        return m_presult->next_result();
    else
        return false;
}

int DBHandle::get_int_result(int index , int &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in Connection is Invalidable! execute query SQL first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_int_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    return 0;
}

int DBHandle::get_string_result(int index , std::string &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in Connection is Invalidable! execute query SQL first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_string_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    return 0;
}

int DBHandle::get_long_result(int index , long long int &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in Connection is Invalidable! execute query SQL first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_long_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    return 0;
}

int DBHandle::get_double_result(int index , double &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in Connection is Invalidable! execute query SQL first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_double_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    return 0;
}

int DBHandle::get_blob_result(int index , const void *&val , int &len)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in Connection is Invalidable! execute query SQL first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_blob_result(index , val , len) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<Connection_getLastError(m_pconn)<<LOG_END;
        return -1;
    }
    return 0;
}

int DBHandle::get_cloumn_count()
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn ERROR ! execute query SQL and get result first !"<<LOG_END;
        return -1;
    }

    return m_presult->get_cloumn_count();
}

int DBHandle::get_cloumn_name(int index , std::string &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn ERROR ! execute query SQL and get result first !"<<LOG_END;
        return -1;
    }

    std::string name;
    if(m_presult->get_cloumn_name(index , name) < 0)
    {
        LOG_ERR_INSIDE<<"Get Cloumn name of index "<<index<<" error : out of range!"<<LOG_END;
        return -1;
    }
    val = name;
    return 0;
}

long DBHandle::get_cloumn_size(int index)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn ERROR ! execute query SQL and get result first !"<<LOG_END;
        return -1;
    }
    
    if(m_presult->get_cloumn_size(index) < 0)
    {
        LOG_ERR_INSIDE<<"Get Cloumn size of index "<<index<<" error : out of range!"<<LOG_END;
        return -1;
    }

    return 0;
}

void DBHandle::finish_prepare(DBPreStatementHandle *pre)
{
    std::list<DBPreStatementHandle *>::iterator it;
    for(it = m_lpre.begin() ; it != m_lpre.end() ; ++ it)
    {
        if(*it == pre)
            break;
    }

    if(it == m_lpre.end())
    {
        LOG_ERR_INSIDE<<"Can not find this prepare handle in connection list !"<<LOG_END;
        return ;
    }

    m_lpre.erase(it);
    delete pre;
    pre = NULL;
}
