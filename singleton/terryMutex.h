/*
 * =====================================================================================
 *
 *       Filename:  terryMutex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/12 12:20:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _H_TERRY_MUTEX_H_
#define _H_TERRY_MUTEX_H_

#include<pthread.h>
#include<assert.h>

#ifdef _USE_THREAD_
#define MUTEX_LOCK(cTerryMutex) (cTerryMutex.lock())
#define MUTEX_UNLOCK(cTerryMutex) (cTerryMutex.unlock)

#else
#define MUTEX_LOCK(cTerryMutex) 
#define MUTEX_UNLOCK(cTerryMutex)
#endif

class terryMutex
{
    public:
        terryMutex(const pthread_mutexattr_t *);
        ~terryMutex();
        void lock();
        void unlock();

    private:
#ifdef _USE_THREAD_
        pthread_mutex_t m_terryMutex;
        pthread_mutexattr_t *p_attr;
#endif
};


#endif
