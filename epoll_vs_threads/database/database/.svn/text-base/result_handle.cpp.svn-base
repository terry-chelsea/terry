/*
 * =====================================================================================
 *
 *       Filename:  result_handle.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/13/13 21:18:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "result_handle.h"

#define LOG_ERR_INSIDE (std::cerr)
#define LOG_WARNING_INSIDE (std::cerr)
#define LOG_INFO_INSIDE (std::cout)
#define LOG_END         (std::endl)

DBResultHandle::DBResultHandle(ResultSet_T res)
    :m_pres(res)
{
    m_iColunm = ResultSet_getColumnCount(res);
}

DBResultHandle::~DBResultHandle()
{
    m_pres = NULL;
    m_iColunm = 0;
}

int DBResultHandle::get_int_result(int index , int &val)
{
    int ret = -1;
    TRY
    {
        ret = ResultSet_getInt(m_pres , index);
    }
    CATCH(SQLException)
    {
        return -1;
    }
    END_TRY;
    val = ret;
    return 0;
}

int DBResultHandle::get_string_result(int index , std::string &val)
{
    const char *ret = NULL;
    TRY
    {
         ret = ResultSet_getString(m_pres , index);
    }
    CATCH(SQLException)
    {
        return -1;
    }
    END_TRY;

    if(ret != NULL)
        val = std::string(ret);
    return 0;
}

int DBResultHandle::get_long_result(int index , long long int &val)
{
    long long int ret = -1;
    TRY
    {
        ret = ResultSet_getLLong(m_pres , index);
    }
    CATCH(SQLException)
    {
        return -1;
    }
    END_TRY;
    val = ret;
    return 0;
}

int DBResultHandle::get_double_result(int index , double &val)
{
    double ret = 0.0;
    TRY
    {
        ret = ResultSet_getDouble(m_pres , index);
    }
    CATCH(SQLException)
    {
        return -1;
    }
    END_TRY;
    val = ret;
    return 0;
}

int DBResultHandle::get_blob_result(int index , const void *&val , int &len)
{
    const void *ret = NULL;
    int ret_len = 0;
    TRY
    {
        ret = ResultSet_getBlob(m_pres , index , &ret_len);
    }
    CATCH(SQLException)
    {
        return -1;
    }
    END_TRY;
    val = ret;
    len = ret_len;
    return 0;
}

int DBResultHandle::get_cloumn_count()
{
    return m_iColunm;
}

int DBResultHandle::get_cloumn_name(int index , std::string &val)
{
    if(index < 0 || index > m_iColunm)
    {
        return -1;
    }

    std::string name;
    name = ResultSet_getColumnName(m_pres , index);
    val = name;
    return 0;
}

long DBResultHandle::get_cloumn_size(int index)
{
    if(index < 0 || index > m_iColunm)
    {
        return -1;
    }

    return ResultSet_getColumnSize(m_pres , index);
}

bool DBResultHandle::next_result()
{
    if(ResultSet_next(m_pres))
        return true;
    else
        return false;
}
