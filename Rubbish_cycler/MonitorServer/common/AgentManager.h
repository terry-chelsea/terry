#ifndef _AGENTMANAGER_H
#define _AGENTMANAGER_H
#include "Singleton.h"
#include "TCPAgent.h"
#include <map>
#include <tr1/memory>

const uint32_t INIT_INDEX = 1;
const uint32_t MAX_INDEX = 0xFFFFFFFE;
const int TRY_TIMES = 10;

class TCPAgent;

//for every agent an index...
class AgentManager:public Singleton<AgentManager>
{
protected:
    AgentManager();
    ~AgentManager();
    friend class Singleton<AgentManager>;

private :
    int addAgent(TCPAgent * );
    void deleteAgent(uint32_t );
    uint32_t generate_index();
    void add_recycler_agent(TCPAgent *&);

public:
    TCPAgent* getAgent(uint32_t );

    //just create empty agent...later set something...
    TCPAgent* createAgent(Epoll* pEpoll);

    //create an client agent , then use connect to create socket...
    TCPAgent* createAgent(const SocketAddress& addr, Epoll* pEpoll);

    //create server TCPAgent , use socket created by accept...
    TCPAgent* createAgent(const TCPSocket& sock, const SocketAddress& addr, Epoll* pEpoll);

    int recyclerAgent(uint32_t );

    int recyclerAgent(Agent *&);
    int recyclerAgent(TCPAgent *&);

    int clean_agent();

private:
    std::map<uint32_t , TCPAgent*> m_TCPAgentMap;

    std::list<TCPAgent *> m_recycler_list;

    uint32_t m_index;
    int   m_list_agent_num;
};
#endif

