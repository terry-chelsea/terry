/*
 * =====================================================================================
 *
 *       Filename:  Task.h
 *
 *    Description:  Task 的基类
 *
 *        Version:  1.0
 *        Created:  11/15/12 12:14:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_TERR_TASK_H_
#define _H_TERR_TASK_H_

#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <tr1/functional>
#include "SCAgent.h"

//Task都是由Agent创建出来的，所以保留着Agent的index,这个index是创建者Agent的句柄
//之所以不保留Agent的指针，是因为Agent可能因为对端的关闭而析构
//如果这时候在回调Agent设置的回调函数，就会产生段错误，
//因此每次在回调的时候首先查看这个Agent是否存在，如果不存在就直接将Task析构
//如果能够查找到Agent，这样就能够执行回调函数了。
//每个Task可以看成是一个任务的执行，它只给调用者一个回调的接口
//这个回调函数只有一个参数标志Task是否执行成功，具体的操作交给上层执行
//但是Task完成的操作需要保证事物的ACID，保证原子性

//注意Task的创建者可以使任何类型的Agent，但一般是ListenAgent创建的被动Agent
//Task创建的是SCAgent，一般需要主动连接对端

class Task 
{
    public :
        Task(uint32_t agent_index);

        virtual ~Task();

        virtual int init()
        {
            return 0;
        }
        virtual int init(std::tr1::function<void (bool)> ret_callback)
        {
            m_task_cb = ret_callback;
            return 0;
        }

        void do_callback(bool ret);
        
        virtual int parse_callback_ret(void *req , RET_TYPE type);

        virtual SCAgent *create_new_agent(uint32_t ip , uint16_t port);

        virtual SCAgent *create_new_agent(const std::string& ip_str , uint16_t port);

    protected :
        //创建者的Agent的句柄
        uint32_t m_father_agent_index;
        //创建者提供的回调函数
        std::tr1::function<void (bool)> m_task_cb;
};

#endif
