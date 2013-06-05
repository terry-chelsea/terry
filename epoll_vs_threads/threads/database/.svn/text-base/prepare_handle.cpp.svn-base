/*
 * =====================================================================================
 *
 *       Filename:  prepare_handle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 15:01:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "prepare_handle.h"
#include "Database_handle.h"
#include "result_handle.h"

#define LOG_ERR_INSIDE  (std::cerr)
#define LOG_WARNING_INSIDE (std::cerr)
#define LOG_INFO_INSIDE (std::cout)
#define LOG_END         (std::endl)

DBPreStatementHandle::DBPreStatementHandle(DBHandle *conn , PreparedStatement_T pre , std::string sql)
    :m_ppre(pre) , 
    m_presult(NULL) , 
    m_sprepare_sql(sql) , 
    m_pconn(conn)
{
}

DBPreStatementHandle::~DBPreStatementHandle()
{
    if(m_presult != NULL)
    {
        delete m_presult;
        m_presult = NULL;
    }
}

int DBPreStatementHandle::set_string_parameter(int index , std::string para)
{
    TRY
    {
        PreparedStatement_setString(m_ppre , index , para.c_str());
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Set string parameter to index "<<index<<" and value "<<para<<" into SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;
    return 0;
}

int DBPreStatementHandle::set_int_parameter(int index , int para)
{
    TRY
    {
        PreparedStatement_setInt(m_ppre , index , para);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Set int parameter to index "<<index<<" and value "<<para<<" into SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;
    return 0;
}

int DBPreStatementHandle::set_long_parameter(int index , long long int para)
{
    TRY
    {
        PreparedStatement_setLLong(m_ppre , index , para);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Set long parameter to index "<<index<<" and value "<<para<<" into SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;
    return 0;
}

int DBPreStatementHandle::set_double_parameter(int index , double para)
{
    TRY
    {
        PreparedStatement_setDouble(m_ppre , index , para);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Set double parameter to index "<<index<<" and value "<<para<<" into SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;

    return 0;
}

int DBPreStatementHandle::set_blob_parameter(int index , const void *para , int len)
{
    TRY
    {
        PreparedStatement_setBlob(m_ppre , index , para , len);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Set blob parameter to index "<<index<<" and length "<<len<<" into SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;

    return 0;
}

int DBPreStatementHandle::execute_statement()
{
    TRY
    {
        PreparedStatement_execute(m_ppre);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Execute perpare statement SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;

    return 0;
}

int DBPreStatementHandle::execute_query_statement()
{
    ResultSet_T ret = NULL;
    if(m_presult != NULL)
    {
        delete m_presult;
        m_presult = NULL;
    }
    TRY
    {
        ret = PreparedStatement_executeQuery(m_ppre);
    }
    CATCH(SQLException)
    {
        LOG_ERR_INSIDE<<"Execute prepare query statement SQL : "<<m_sprepare_sql<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    END_TRY;

    m_presult = new DBResultHandle(ret);
    return 0;
}

bool DBPreStatementHandle::next_result()
{
    if(m_presult != NULL)
        return m_presult->next_result();
    else
        return false;
}

int DBPreStatementHandle::get_int_result(int index , int &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in PreparedStatement is Invalidable! execute query first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_int_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    return 0;
}

int DBPreStatementHandle::get_string_result(int index , std::string &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in PreparedStatement is Invalidable! execute query first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_string_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    return 0;
}

int DBPreStatementHandle::get_long_result(int index , long long int &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in PreparedStatement is Invalidable! execute query first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_long_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    return 0;
}

int DBPreStatementHandle::get_double_result(int index , double &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in PreparedStatement is Invalidable! execute query first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_double_result(index , val) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    return 0;
}

int DBPreStatementHandle::get_blob_result(int index , const void *&val , int &len)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Result in PreparedStatement is Invalidable! execute query first!"<<LOG_END;
        return -1;
    }

    if(m_presult->get_blob_result(index , val , len) < 0)
    {
        LOG_ERR_INSIDE<<"Get Result of index "<<index<<" error : "<<m_pconn->last_error()<<LOG_END;
        return -1;
    }
    return 0;
}

int DBPreStatementHandle::get_cloumn_count()
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn IN PreparedStatement ERROR ! execute query and get result first !"<<LOG_END;
        return -1;
    }

    return m_presult->get_cloumn_count();
}


int DBPreStatementHandle::get_cloumn_name(int index , std::string &val)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn IN PreparedStatement ERROR ! execute query and get result first !"<<LOG_END;
        return -1;
    }

    std::string name;
    if(m_presult->get_cloumn_name(index , name) < 0)
    {
        LOG_ERR_INSIDE<<"Get Cloumn name IN PreparedStatement of index "<<index<<" error : out of range!"<<LOG_END;
        return -1;
    }
    val = name;
    return 0;
}

long DBPreStatementHandle::get_cloumn_size(int index)
{
    if(NULL == m_presult)
    {
        LOG_ERR_INSIDE<<"Get Result Cloumn IN PreparedStatement ERROR ! execute query and get result first !"<<LOG_END;
        return -1;
    }
    
    if(m_presult->get_cloumn_size(index) < 0)
    {
        LOG_ERR_INSIDE<<"Get Cloumn size IN PreparedStatement of index "<<index<<" error : out of range!"<<LOG_END;
        return -1;
    }

    return 0;
}

