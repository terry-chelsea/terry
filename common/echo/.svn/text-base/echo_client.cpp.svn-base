/*
 * =====================================================================================
 *
 *       Filename:  echo_client.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/22/13 21:10:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "../TCPAgent.h"
#include "../Epoll.h"
#include "../debug.h"
#include "../BaseHeader.h"
#include "../BaseReq.h"
#include "../SocketAddress.h"
#include "../Client.h"


#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>

#define MAX_CLIENT 10000
#define MAX_LINE  1024

using namespace std;

#define BASE_LINE "I am agent of address %p and this is times %d !"

string get_input_line()
{
    string ret;
    char init_line[MAX_LINE];
    memset(init_line , 0 , MAX_LINE);

    char *ptr = fgets(init_line , MAX_LINE , stdin);
    if(!ptr)
    {
        static int count = 0;
        LOG_INFO("get EOF line !");
        ++ count;
        if(count > 1)
            exit(0);

        return ret;
    }

    int len = strlen(ptr);
    init_line[len - 1] = '\0';
    len --;
    ret.assign(ptr , len);

    return ret;
}

string get_line_2(void *arg , int times)
{
    char line[MAX_LINE];
    memset(line , 0 , MAX_LINE);

    int len = snprintf(line , MAX_LINE , BASE_LINE , arg , times);

    line[len] = '\0';

    string ret(line , len);
    return ret;
}

void echo_read_data(const Context &ctx)
{
    TCPAgent *agent = ctx.agent;
    int len = ctx.para.req_in.m_msgHeader.length;

    string in_data(ctx.para.req_in.ioBuf , len);
    LOG_INFO("Get line from server : \"%s\"" , in_data.c_str());


    int para1 = ctx.para.req_in.m_msgHeader.para1;
    string out_data = get_line_2(agent , ++para1);
    agent->send_data(0 , 0 , para1 , 0 ,out_data);
}

void echo_write_data(const Context &ctx)
{
    int ret = ctx.para.result;
    if(ret)
        LOG_INFO("write data to buffer success !");
    else 
        LOG_INFO("Write data to buffer failed !");
}

int echo_connect_after(const Context &ctx)
{
    bool ret = ctx.para.result;
    TCPAgent *agent = ctx.agent;
    if(!ret)
    {
        LOG_INFO("connect to server error !");
        return -1;
    }

    string out_data = get_line_2(agent , 0);
    agent->send_data(0 , 0 , 0 , 0 , out_data);
    return 0;
}

int echo_recycler(const Context &ctx)
{
    LOG_INFO("recycler of agent : %p" , ctx.agent);
    return 0;
}

const Agent_operations operations = {
    "echo client agent" , 
    echo_connect_after , 
    echo_recycler  ,
    echo_read_data , 
    echo_write_data , 
};


int main(int argc , char *argv[])
{
    if(argc != 3)
    {
        LOG_ERROR("./Usage : ./client ip port !");
        return -1;
    }

    char *ip = argv[1];
    unsigned short port = atoi(argv[2]);

    SocketAddress addr(ip , port);

    Epoll *epll = new Epoll();
    if(epll->initialize(MAX_CLIENT) < 0)
    {
        LOG_ERROR("epoll initialize error !");
        return -1;
    }

    Client *client = new Client(epll);
    client->set_operations(operations);
    if(client->connect(addr) < 0)
    {
        LOG_ERROR("Agent connect error !");
        return -1;
    }

    
    epll->run();

    return 0;
}

