/*
 * =====================================================================================
 *
 *       Filename:  singletonBase.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/12 12:41:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _H_SINGLETON_BASE_H_
#define _H_SINGLETON_BASE_H_

#include "terryMutex.h"

template<typename T>
class SingletonBase
{
    public:
        static T* getInstance();

    protected:
        SingletonBase()
        {

        }
        virtual ~SingletonBase()
        {

        }
        
    protected:
        static T* p_singleton;
        static terryMutex m_mutex;
};

template<typename T>
T* SingletonBase<T>::p_singleton = NULL;

template<typename T>
terryMutex SingletonBase<T>::m_mutex(NULL);

template<typename T>
inline T* SingletonBase<T>::getInstance()
{
   if(NULL == p_singleton) 
   {
       MUTEX_LOCK(m_mutex);
       if(NULL == p_singleton)
       {
           p_singleton = new T;
       }
       MUTEX_UNLOCK(m_mutex);
   }

   return p_singleton;
}

#endif
