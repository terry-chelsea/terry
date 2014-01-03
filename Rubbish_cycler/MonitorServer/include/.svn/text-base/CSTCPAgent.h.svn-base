/*
 * =====================================================================================
 *
 *       Filename:  CSTCPAgent.h
 *
 *    Description:  CS中所有Agent的基类，不适用引用计数，但是使用编号 
 *
 *        Version:  1.0
 *        Created:  11/07/12 17:29:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_CS_TCPAGENT_H_
#define _H_CS_TCPAGENT_H_

#include "../common/comm/comm.h"

typedef struct MsgHeader MsgHeader;
#define ERROR_INDEX ((uint32)(-1))

//所有Agent的基类，每次创建的是都会讲它加入到map中
//每次析构的时候将它从map中移除
//这样每次只需要通过index句柄查看到对于的SCAgent了，对于派生类的Agent
//可以使用dynamic_cats进行转换，这样可以防止非法的析构，因为如果Agent如果被非法析构了
//那么在析构的时候会在map中erase了，那么查找将会失败，对于Agent非法析构的情况，由调用者处理
//P.S.非法析构是由于基类引起的，例如对端发送RST、FIN报文都会引起Agent的自动析构

class CSTCPAgent : public TCPAgent
{
    public :
        //用户创建的主动连接的Agent
        CSTCPAgent(const SocketAddress &oppo_addr , Epoll *epl);
        //TCPListenAgent创建的接受对方连接的Agent
        CSTCPAgent(const TCPSocket &sock , const SocketAddress &oppo_addr 
                , Epoll *epl);

        virtual ~CSTCPAgent();

        //获取Agent的index句柄，通过index查找Agent
        uint32_t get_agent_index()
        {
            return m_index;
        }

        virtual int init()
        {
            return 0;
        }

        //打包并发送，将报文头和负载数据打包并发送到应用层缓冲区
        void do_package(MsgHeader &header , const std::string &out_buf);
        //构造发送的报文，将报文头和负载数据打包到一个数据包里面
        void do_package(MsgHeader &header , const std::string &out_buf , std::string &out_string);

        //发送一项数据，包括报文头的内容和负载数据
        void reply_request(uint32_t cmd , uint32_t error , uint32_t para1 , uint32_t para12 , const std::string &out_data);

    protected :
        void set_agent_index(uint32_t index)
        {
            m_index = index;
        }

    protected :
        uint32_t m_index;
};

#endif
