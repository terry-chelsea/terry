#include "agent.h"
#include "TCPAgent.h"
#include "AgentManager.h"
#include "debug.h"
#include "SocketAddress.h"
#include "Task.h"
#include "protocol.h"

#include <tr1/functional>
#include <time.h>

struct Agent_operations ca_operatons = 
{
    "ca agent for login and authentication" , 
    ca_connect_back , 
    ca_recycler_back , 
    ca_read_back , 
    NULL
};

void default_callback(Task *tsk);

int ca_connect_back(const struct Context &ctx)
{
    TCPAgent *agent = ctx.agent;
    if(NULL == agent)
    {
        LOG_WARNING("Agent is NULL in ca connect after");
        return 0;
    }

    bool ret = ctx.para.result;
    SocketAddress peer;
    agent->getOppAddr(peer);
    if(ret)
    {
        LOG_INFO("Connection success from address %s and port %d" , peer.getIP() , peer.getPort());
        return 0;
    }
    else 
    {
        LOG_ERROR("Connection to address %p and port %d failed !" , peer.getIP() , peer.getPort());
        return -1;
    }
    return 0;
}

int ca_recycler_back(const struct Context &ctx)
{
    TCPAgent *agent = ctx.agent;
    if(NULL == agent)
    {
        LOG_WARNING("Agent is NULL in ca recycler after");
        return 0;
    }

    SocketAddress peer;
    agent->getOppAddr(peer);
    
    return 0;
}

void ca_read_back(const struct Context &ctx)
{
    TCPAgent *agent = ctx.agent;
    if(NULL == agent)
    {
        LOG_WARNING("Agent is NULL in ca read back");
        return ;
    }
    
    InReq *req = ctx.para.req_in;
    struct MsgHeader header = req->m_msgHeader;
    int len = header.length;
    char *buf = new char[len];
    memcpy(buf , req->ioBuf , len);

    struct TPara para = {header , buf , agent->get_agent_index()};
    
    Task *tsk = NULL;
    
    uint32_t cmd = header.cmd;
    int ack_cmd = 0;
    uint32_t error_code = 0;
    std::string out;
    switch(cmd)
    {
        error_code = CA_INSIDE_ERROR;
        case MSG_SYS_CA_USER_LOGIN :
        {
            ack_cmd = MSG_SYS_CA_USER_LOGIN_ACK;

            tsk = new LoginTask();
            if(NULL == tsk)
            {
                LOG_ERROR("Create Login Task error...");
                goto RET ;
            }
            if(tsk->init(para , default_callback) < 0)
            {
                LOG_ERROR("Login task initialize error !");
                error_code = tsk->get_task_result();
                goto RET2;
            }
            break;
        }
        default : 
        {
            LOG_ERROR("Undefined command type ...");
            ack_cmd = MSG_SYS_CA_USER_SIGNLE_AUTH_ACK;
            error_code = UNDEFINED_CMD_TYPE;
            goto RET;
        }
    }    
    
    return ;
RET2 :
    if(tsk != NULL)
    {
        delete tsk;
        tsk = NULL;
    }
    
RET :
    if(agent->send_data(ack_cmd , error_code , header.para1 , header.para2 , out) < 0)
    {
        LOG_ERROR("OH my god ! error happens everywhere...");
    }
}

void default_callback(Task *tsk)
{
    if(tsk == NULL)
    {
        LOG_WARNING("Task pointer is NULL when task default callback !");
        return ;
    }

    std::string out_data;
    struct TPara para = tsk->get_result_para();
    TCPAgent *agent = AgentManager::getInstancePtr()->getAgent(para.index) ;
    
    if(NULL == agent)
    {
        LOG_WARNING("Agent of index %u has been recycler..." , para.index);
        goto RET;
    }

    if(para.load != NULL)
    {
        out_data.assign(para.load , para.header.length);
    }

    if(agent->send_data(para.header.cmd , 
            tsk->get_task_result() , 
            para.header.para1 , 
            para.header.para2 , 
            out_data) < 0)
    {
        LOG_ERROR("Task callbaqck : send data of agent error !");
    }

RET : 
    if(para.load != NULL)
        delete [] (para.load);
    delete tsk;
    tsk = NULL;
}
