/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.h
 *
 *    Description:  实现线程池的功能，自带两根管道，用于于主线程通信
 *
 *        Version:  1.0
 *        Created:  11/06/12 10:35:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_THREAD_POOL_H_
#define _H_THREAD_POOL_H_

#include "../common/sys/Thread.h"
#include "../common/sys/Singleton.h"
#include <list>
#include "WorkItem.h"

#define DEFAULT_THREAD_NUM   15
#define DEFAULT_ADD_NUM      1
#define MIN_THREAD_NUM       2
#define DEFAULY_STOP_NUM     1

class ThreadPool : public Singleton<ThreadPool> 
{
    friend class Singleton<ThreadPool> ;
    public : 
        ThreadPool();
        ~ThreadPool();

        //主线程读管道的套接字，一般为非阻塞
        int get_main_write_handle()
        {
            return m_main_write_fd;
        }
        //主线程写管道的套接字，一般为非阻塞
        int get_main_read_handle()
        {
            return m_main_read_fd;
        }
        int start(int num = DEFAULT_THREAD_NUM);
        
        int post_request(WorkItem *item);

        //暂停所有线程，使用信号量实现
        int stop();

        //向线程池中添加一些线程
        int add_thread(int num = DEFAULT_ADD_NUM);

        //暂停部分线程
        int stop_thread(int num = DEFAULY_STOP_NUM);

        int get_thread_num()
        {
            return m_thread_num;
        }

    protected :
        class ThreadHandler : public Thread
        {
            public : 
                ThreadHandler(ThreadPool *pool);

            protected :
                void run();

            private :
                ThreadPool *m_thread_pool;
        };

    private :
        int m_thread_num;
        int m_main_write_fd;
        int m_main_read_fd;
        int m_thread_write_fd;
        int m_thread_read_fd;

        std::list<Thread *> m_thread_list;
        Mutex m_mutex;
};


#endif
