/*
 * =====================================================================================
 *
 *       Filename:  SCAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/08/12 11:27:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "SCAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"

SCAgent::SCAgent(const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(addr , epl) , 
    m_ret(UNKOWN_ERROR) ,
    m_is_call_cb(false) , 
    m_data(NULL)
{

}

SCAgent::~SCAgent()
{

}

int SCAgent::init(std::tr1::function<void (void * , RET_TYPE , void *)> result_cb , const string &data , void *s_data)
{
    //保存回调函数和用户传递的私有数据
    m_ret_cb = result_cb;
    m_data = s_data;

    //向对端进行连接
    if(TCPAgent::connect(TCPAgent::m_Addr) < 0)
    {
        LOG_ERROR("SCAgent::connect IP : " + m_Addr.ip 
                + " Port : " + ushort_to_str(m_Addr.getPort()) + " error !");

        //连接失败回调，一般出现错误的回调都会回收Agent
        do_call_back(CONNECT_ERROR , NULL);

        return -1;
    }

    m_write_data = data;
    return 0;
}

int SCAgent::connectAfter(bool ret)
{
    //连接失败
    if(false == ret)
    {
        LOG_ERROR("SCAgent::Connect_after : connect IP : " + m_Addr.ip 
                + " Port : " + ushort_to_str(m_Addr.getPort()) + " error !");

        do_call_back(CONNECT_ERROR , NULL);
        return -1;
    }
    else
    {
        //这里创建的内存在发送完成之后基类释放
        int send_length = m_write_data.size();
        char *send_buf = new char[send_length];
        memcpy(send_buf , m_write_data.c_str() , send_length);

        //数据发送工作可以交给reply_request函数完成
        if(TCPAgent::writeDynData(send_buf , send_length , NULL) < 0)
        {
            LOG_ERROR("SCAgent::write data to user buf error ! Data length " 
                    + int_to_str(send_length));

            do_call_back(WRITE_ERROR , NULL);

            return -1;
        }

        return 0;
    }
}

void SCAgent::writeBack(bool ret)
{
    if(false == ret)
    {
        LOG_ERROR("SCAgent::send data to peer IP : " + m_Addr.ip + 
                " Port : " + ushort_to_str(m_Addr.getPort()) + " error !");

        do_call_back(WRITE_ERROR , NULL);

        return ;
    }
    else 
    {
        LOG_EVENT("SCAgent::send data to IP " + m_Addr.ip + 
                " Port : " + ushort_to_str(m_Addr.getPort()) + " Successfully !");

        return ;
    }
}

//直接交给回调函数执行，回调函数可能需要将这个Agent释放
void SCAgent::readBack(InReq &req)
{
    //读取到结果之后直接交给用户处理
    do_call_back(SUCCESSFULLY , &req);
}

int SCAgent::recycler()
{
    //首先判断是否已经产生了回调，如果没有就执行回调，这种情况一般不会发生
    //除非对端主动关闭连接，这样Agent就会析构，recycler会被调用
    //但是一般情况下，SCAgent会主动关闭连接
    if(false == m_is_call_cb)
    {
        do_call_back(UNKOWN_ERROR , NULL);
    }

    TCPAgent::recycler();

    return 0;
}

void SCAgent::do_call_back(RET_TYPE type , void *req)
{
    m_ret = type;
    m_ret_cb(req , type , m_data);

    m_is_call_cb = true;
}
