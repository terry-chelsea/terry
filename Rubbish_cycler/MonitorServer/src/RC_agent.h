/*
 * =====================================================================================
 *
 *       Filename:  RC_agent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/07/13 10:21:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 * =====================================================================================
 */

#ifndef _H_RC_HEARTBEAT_AGENT_H_
#define _H_RC_HEARTBEAT_AGENT_H_

//垃圾回收使用的心跳Agent，需要能满足一下的需求：
//1、在读取到数据的时候，执行上层的回调函数，并且附带IP信息
//2、如果出现错误，也需要交给上层处理。
//并且可以随时发送数据,例如定时器超时之后需要发送心跳信息
//在需要发送命令的时候，发送命令报文...

#include "CSTCPAgent.h"
#include <tr1/functional>

typedef enum
{
    RC_SUCCESSFULLY = 0 ,
    RC_CONNECT_FAILED = 2, 
    RC_CONNECT_SUCCESS = 3,
    RC_WRITE_FAILED = 4, 
    RC_UNKNOW_ERROR = 5,
}RC_TYPE;

class RCAgent : public CSTCPAgent
{
    public : 
        RCAgent(Epoll *epl);
        ~RCAgent();

        int init(uint32_t ip , uint16_t port , 
                std::tr1::function<void (RC_TYPE , void *)> cb);

        int connectAfter(bool ret);

        void readBack(InReq &req);

        void writeBack(bool ret);

        int delete_agent()
        {
            return TCPAgent::recycler();
        }

        void *get_data()
        {
            return m_data;
        }

        int recycler();

        int write_data(const void *buf , int len);

        void do_callback(RC_TYPE ret , void *info)
        {
            m_callback(ret , info);
        }

        uint32_t agent_ip()
        {
            return m_this_ip;
        }

        std::string getIP()
        {
            return m_Addr.getIP();
        }
    
    private :
        std::tr1::function<void (RC_TYPE , void *)> m_callback;
        bool  m_recycler_flag;
        
        uint32_t m_this_ip;
        void *m_data;
};

#endif
