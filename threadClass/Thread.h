/*
 * =====================================================================================
 *
 *       Filename:  Thread.h
 *
 *    Description:  use this class encapsulation a thread...
 *
 *        Version:  1.0
 *        Created:  03/29/12 10:29:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _H_MY_THREAD_H_
#define _H_MY_THREAD_H_

#include<iostream>
#include<pthread.h>

typedef void *(*THREAD_FUNC)(void *);

class Thread
{
    public :
        Thread(string threadName);
        ~Thread();

        string getName()
        {
            return name;
        }
        void setName(string name)
        {
            this.name = name;
        }
        void run(THREAD_FUNC , void *);

    private:
        pthread_t n_tid;
        string    s_name;
};

#endif
