/*
 * =====================================================================================
 *
 *       Filename:  RSAgent.h
 *
 *    Description:  短连接Agent，需要回调函数，所有需要主动创建向其他服务器发起连接的请求都通过SCAgent完成
 *
 *        Version:  1.0
 *        Created:  11/07/12 11:57:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry (), fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_SC_AGENT_H_
#define _H_SC_AGENT_H_

//使用短连接的方式，一般一个Agent的生命周期如下：create->init->connect->
//connectAfter->write->writeBack->waiting peer data->readBack->callback->recycler
//最后delete这个对象

#include "CSTCPAgent.h"
#include <iostream>
#include <list>
#include <string>
#include <tr1/functional>

typedef enum
{
    SUCCESSFULLY = 0 ,
    CONNECT_ERROR = 1 ,
    WRITE_ERROR = 2 ,
    UNKOWN_ERROR = 3  
}RET_TYPE;

//SCAgent可以作为一个主动连接的Agent
//它既可以作为长连接，也可以作为短连接使用
//对于短连接，只需要创建一个SCAgent，然后调用init函数初始化连接对端
//然后数据的写入和回复的读取交给基类自动完成，调用端只会在适当的时候被毁掉
//如果是错误的回调，需要创建者回收SCAgent，如果成功，那么一次通信成功，对于短连接，
//对端可以回收SCAgent了。
//对于长连接，第一次也是init直接等到SCAgent的回调，如果错误的回调，将仍然由创建者回收
//如果成功，创建者可以继续进行数据的读写，这时候调用reply_request函数写出数据
//等待下一次被回调

class SCAgent : public CSTCPAgent
{
    public :
        SCAgent(const SocketAddress &addr , Epoll *epl);
        ~SCAgent();

        //初始化函数，函数设置一个回调函数，函数有三个参数
        //第一个参数是用户设置的回调函数
        //第二个参数是用户需要发送的数据
        //第三个参数是用户传递的私有数据，回调的时候原样返回给用户，由用户解析
        //第三个参数需要注意传递的是指针，小心内存错误
        int init(std::tr1::function<void (void * , RET_TYPE , void *)> result_cb , const string &data , void *);

        int connectAfter(bool ret);

        void readBack(InReq &req);

        void writeBack(bool ret);

        int recycler();

        void do_call_back(RET_TYPE type , void *);

        void set_write_data(const std::string &out_data)
        {
            m_write_data = out_data;
        }
   
    private :
        //回调函数有三个参数，第一个参数是回调者传递的参数，一般为NULL，或者readBack的结果
        //第一个参数是用户读取的数据InReq的内容，它会在readBack之后析构，所以需要确保回调
        //函数在readBack函数返回之前执行
        //第二个参数是回调函数返回的类型，作为结果字段
        //第三个参数是用户传递的参数，原样传递回去，由用户解析

        std::tr1::function<void (void * , RET_TYPE , void *)> m_ret_cb;
        //待发送的数据
        std::string m_write_data;
        //请求的结果
        RET_TYPE  m_ret;
        //是否已经调用回调函数
        bool  m_is_call_cb;

        void *m_data;
};

#endif
