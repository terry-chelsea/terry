/*
 * =====================================================================================
 *
 *       Filename:  CSTCPAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 17:39:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "CSTCPAgent.h"
#include "AgentRegister.h"

CSTCPAgent::CSTCPAgent(Epoll *epl)
    :TCPAgent(epl)
{
    uint32_t index = AgentRegister::getInstance()->add_agent(this);

    m_index = index;
}

CSTCPAgent::CSTCPAgent(const SocketAddress &oppo_addr , Epoll *epl)
    :TCPAgent(oppo_addr , epl)
{
    uint32_t index = AgentRegister::getInstance()->add_agent(this);

    m_index = index;
}

CSTCPAgent::CSTCPAgent(const TCPSocket &sock , const SocketAddress &oppo_addr
         , Epoll *epl)
    :TCPAgent(sock , oppo_addr , epl)
{
    uint32_t index = AgentRegister::getInstance()->add_agent(this);

    m_index = index;
}

CSTCPAgent::~CSTCPAgent()
{
    //只会从map中清楚这个Agent
    AgentRegister::getInstance()->delete_agent(m_index);
    m_index = 0;
}

//打包数据并且发送出去，动态创建的缓冲区由基类释放
void CSTCPAgent::do_package(MsgHeader &header , const std::string &out_buf)
{
    //再次保证长度正确
    header.length = out_buf.size();
    const int out_len = out_buf.size();
    const int buf_len = sizeof(header) + out_len;
    char *buf = new char[buf_len];

    memcpy(buf , &header , sizeof(header));
    if(out_len != 0)
    {
        memcpy(buf + sizeof(header) , out_buf.c_str() , out_len);
    }

    TCPAgent::writeDynData(buf , buf_len , NULL);
}

//将报文头和负载数据打包到一个新的string上,需要保证string只包含报文头和负载数据
void CSTCPAgent::do_package(MsgHeader &header , const std::string &out_buf , std::string &out_string)
{
    out_string.clear();

    //再次保证长度正确
    header.length = out_buf.size();
    std::string temp_str((const char *)&header , sizeof(header));
    out_string += temp_str;
    out_string += out_buf;
}

void show_header(MsgHeader &header)
{
    std::cerr<<"#######Header###########"<<std::endl;
    std::cerr<<"cmd : "<<header.cmd<<std::endl;
    std::cerr<<"length : "<<header.length<<std::endl;
    std::cerr<<"error : "<<header.error<<std::endl;
    std::cerr<<"para1 : "<<header.para1<<std::endl;
    std::cerr<<"para2 : "<<header.para2<<std::endl;
    std::cerr<<"##########End###########"<<std::endl;
}

//根据传递的参数构造报文头，发送到应用层缓冲区
void CSTCPAgent::reply_request(uint32_t cmd , uint32_t error , uint32_t para1 , uint32_t para2 , const std::string &out_data)
{
    MsgHeader header;
    memset(&header , 0 , sizeof(header));

    header.cmd = cmd;
    header.para1 = para1;
    header.para2 = para2;
    header.error = error;
    header.length = out_data.size();

//    show_header(header);
    do_package(header , out_data);
}

