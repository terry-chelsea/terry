
#include "../Server.h"
#include "../TCPAgent.h"
#include "../debug.h"
#include "../Epoll.h"
#include "../SocketAddress.h"
#include "../BaseReq.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>

using namespace std;

void read_data(const struct Context &ctx)
{
	int len = ctx.para.req_in.m_msgHeader.length;
	TCPAgent *agent = ctx.agent;

    int para1 = ctx.para.req_in.m_msgHeader.para1;
    int para2 = ctx.para.req_in.m_msgHeader.para2;
	std::string data;
	if(len <= 0)
	{
		data = string("empty data !");
	}
	else 
	{
		data = string(ctx.para.req_in.ioBuf , len);
	}

	if(agent->send_data(0 , 0 , para1 , para2 , data) < 0)
		exit(-1);

//	LOG_INFO("Send to client : %s" , data.c_str());
}

void write_data(const struct Context &ctx)
{
	if(ctx.para.result)
    {
//		LOG_INFO("Write data to client success !");
    }
	else 
    {
		LOG_WARNING("Write data to client failed !");
    }
}

int recycler_agent(const struct Context &ctx)
{
//	LOG_INFO("Recycler of agent address : %p" , (ctx.agent));
	return 0;
}

int connect_agent(const struct Context &ctx)
{
	if(ctx.para.result)
    {
//		LOG_INFO("Connection of agent address : %p" , (ctx.agent));
	}
	else 
		LOG_WARNING("COnnection error !");
	return 0;
}

const struct Agent_operations echo_operations = {
	"echo agent" , 
	connect_agent ,
	recycler_agent , 
	read_data , 
	write_data
};

int main(int argc , char *argv[])
{
	if(argc != 2)
	{
		cerr<<"Usage : ./echo_server port"<<endl;
		return -1;
	}
	int port = atoi(argv[1]);
	if((port < 0) || (port >= (1 << 16)))
	{
		LOG_ERROR("Inlegel port : %d" , port);
		return -1;
	}

	Epoll *epl = new Epoll();
	if(epl->initialize(10000) < 0)
	{
		LOG_ERROR("epoll initialize error !");
		return -1;
	}

	SocketAddress addr("0" , port);
	Server *echo = new Server(epl , echo_operations);
	if(echo->init(addr) < 0)
	{
		LOG_ERROR("Echo server init error !");
		return -1;
	}

	while(1)
		epl->run();
}
