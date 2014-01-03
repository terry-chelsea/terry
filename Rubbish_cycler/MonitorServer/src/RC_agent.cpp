/*
 * =====================================================================================
 *
 *       Filename:  RC_agent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/07/13 11:24:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "RC_agent.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "RC_protocol.h"

RCAgent::RCAgent(Epoll *epl) 
    :CSTCPAgent(epl) ,
    m_recycler_flag(false)
{

}

RCAgent::~RCAgent()
{
}

int RCAgent::init(uint32_t ip , uint16_t port , 
        std::tr1::function<void (RC_TYPE , void *)> cb)
{
    m_callback = cb;
    m_this_ip = ip;

    string ip_str;
    if(int_to_string_ip(ip , ip_str) < 0)
    {
        LOG_ERROR("Change interge IP to string failed ...");
        return -1;
    }

    SocketAddress addr(ip_str.c_str() , port);
    if(TCPAgent::connect(addr) < 0)
    {
        LOG_ERROR("RCAgent::Connect to IP : " + ip_str + " Port : " + 
                ushort_to_str(port) + " failed ...");
        
        return -1;
    }

    return 0;
}

int RCAgent::connectAfter(bool ret)
{
    RC_TYPE value = RC_CONNECT_SUCCESS;
    if(!ret)
        value = RC_CONNECT_FAILED;

    do_callback(value , this);

    return value == RC_CONNECT_SUCCESS ? 0 : -1;
}

void RCAgent::readBack(InReq & req)
{
    m_data = &req;
    do_callback(RC_SUCCESSFULLY , this);
}

void RCAgent::writeBack(bool ret)
{
    if(!ret)
        do_callback(RC_WRITE_FAILED , this);
}

int RCAgent::recycler()
{
    if(false == m_recycler_flag)
        do_callback(RC_UNKNOW_ERROR , this);

    TCPAgent::recycler();

    return 0;
}

int RCAgent::write_data(const void *data , int len)
{
    char *send_buf = new char[len];
    if(NULL == send_buf)
    {
        LOG_ERROR("Allocate memory for send data failed !");
        return -1;
    }
    memcpy(send_buf , data , len);

    if(TCPAgent::writeDynData(send_buf , len , NULL) < 0)
    {
        LOG_ERROR("RCAgent::write buffer to socket failed !");
        return -1;
    }

    return 0;
}
