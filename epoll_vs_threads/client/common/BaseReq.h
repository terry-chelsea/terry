#ifndef _BASEREQ_H_
#define _BASEREQ_H_
//#include <cstring>
//#include <iostream>
//#include "Agent.h"
#include "BaseHeader.h"
#include <netinet/in.h>
#include <string>
struct BaseRequest
{
    BaseRequest() {}
    virtual ~BaseRequest() {}
};

struct InReq
{
    MsgHeader m_msgHeader;
    char *ioBuf;
    sockaddr_in oppoAddr;
};

class TCPAgent;
struct Context
{
    union
    {
        bool   result;    //for connectAfter and writeback...
        InReq  *req_in;    //for readBack...
        //recycler need for nothing...
    }para;
    
    TCPAgent *agent;
};

struct TPara
{
    MsgHeader header ; 
    char *   load;
    uint32_t index;    //agent index
};

typedef int (*Int_op)(const struct Context &);
typedef void (*Void_op)(const struct Context &);

struct Agent_operations
{
    std::string name;
    Int_op  connect_after;
    Int_op  recycler_after;
    Void_op read_back;
    Void_op write_back;
};

const struct Agent_operations default_operation = {
    std::string("default operations") , 
    NULL , 
    NULL , 
    NULL , 
    NULL
};

#endif
