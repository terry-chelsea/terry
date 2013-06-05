/*
 * =====================================================================================
 *
 *       Filename:  terryMutex.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/12 12:25:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "terryMutex.h"

#ifdef _USE_THREAD_

terryMutex::terryMutex(const pthread_mutexattr_t *attr)
    :p_attr(attr)
{
    int rtn = pthread_mutex_init(&m_terryMutex , attr);
    assert(0 == rtn);
}

terryMutex::~terryMutex()
{
    int rtn = pthread_mutex_destroy(&m_terryMutex);
    assert(0 == rtn);
}

void terryMutex::lock()
{
    int rtn = pthread_mutex_lock(&m_terryMutex);

    assert(0 == rtn);
}

void terryMutex::unlock()
{
    int rtn = pthread_mutex_unlock(&m_terryMutex);

    assert(0 == rtn);
}

#else

terryMutex::terryMutex(const pthread_mutexattr_t *)
{

}

terryMutex::~terryMutex()
{

}

void terryMutex::lock()
{

}

void terryMutex::unlock()
{

}

#endif
