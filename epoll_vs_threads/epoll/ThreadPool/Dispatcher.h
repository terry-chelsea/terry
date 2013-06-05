/*
 * =====================================================================================
 *
 *       Filename:  Dispatcher.h
 *
 *    Description:  主线程与线程池之间的任务传输者
 *
 *        Version:  1.0
 *        Created:  11/06/12 11:46:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TERRY_DISPATCHER_H_
#define _H_TERRY_DISPATCHER_H_

#include <iostream>
#include <list>
#include <errno.h>

#include "../common/Agent.h"
#include "../common//Epoll.h"
#include "../common/EpollEvent.h"
#include "../common/Singleton.h"
#include "ThreadPool.h"
//使用线程池提供的管道，非阻塞的读写请求
//该对象和线程池是组合关系，都是单例模式实现

class Dispatcher : public Agent , public Singleton<Dispatcher> 
{
    public :
        Dispatcher();
        ~Dispatcher();

        //初始化调度器
        int init(Epoll *epl , ThreadPool *pool);
    
        //下发一个任务到线程池
        void send_request(WorkItem *item);

        //读事件被触发，读取请求处理结果
        int recvData();

        //写事件被触发，写出所有的请求
        int sendData();


        void deal_with_all_recv_request();

    private :
        ThreadPool * m_thread_pool;
        int m_read_fd;
        int m_write_fd;
        std::list<WorkItem *> m_request_list;
        std::list<WorkItem *> m_recv_list;

        //epoll
        Epoll *m_epoll;
        EpollEvent m_read_event;
        EpollEvent m_write_event;

        bool m_open_write;
};

#endif
