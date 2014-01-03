
#ifndef _RUBBISH_RECYCLER_H__
#define _RUBBISH_RECYCLER_H__

#include <iostream>
#include "../common/sys/Singleton.h"
#include "RulerManager.h"
#include "RC_agent.h"
#include <vector>
#include <list>
#include <map>

#define MAX_TOKEN_HOLDS 1
class RCAgent;
class RCCheckHeartbeat;
class RCManager : public Singleton<RCManager>
{
    public :
        RCManager() :
            m_agent_index(0) , 
            m_error_rate(0.0) , 
            m_scale(0) , 
            m_bitmap_size(0) , 
            m_hash_numbers(0) , 
            m_current_token_holders(0) , 
            m_max_token_holders(MAX_TOKEN_HOLDS) , 
            m_timer(NULL) , 
            m_current_state(IDEL_STATE)
        {}

        int get_all_rules(RULE_TYPE type);
        void check_heartbeat_state();

        int connect_all_node_start_rc(const std::string &type);
        void init_paramaters();

        void choice_hash_functions();

        int start_do_recylcer(uint32_t agent_index , unsigned long long scale , double error_rate);
        
        int start_merge_all_bitmap();
        void agent_connect_error(RCAgent *agent);

        void agent_run_error(RCAgent *agent);

        int send_first_command(RCAgent *agent);

        int find_next_bucket(uint32_t ip);

        int start_bucket(RCAgent *agent , int bucket_nr);

        int deal_with_data(void *data , RCAgent *agent);

        void agent_callback(RC_TYPE ret , void *para);
    private :
#define INSIDE_IP_NUMS  5
        typedef enum
        {
            NOT_START = 0 , 
            WORKING , 
            FINISHED
        }BucketState;

        typedef enum
        {
            INITING = 0 ,  //after connect to node ...
            STARTING ,     //sending start bucket command ...
            RECYCLERING ,      //doing bucket recycler...
            STOPPING ,     //stoping , wait for continue command ...
            BLOCKING ,     //waiting for the token ...
            UPLOADING ,    //MU upload the bucket bitmap ...
            DOWNLOADING ,  //SU download the bucket bitmap ...
            WAITING        //wait for the new bucket command ...
        }Node_State;

        struct Rule
        {
            BucketState status;   //0 means not start , 1 means working , 2 means finish...
            uint32_t    bucket_nr;                 //桶号
            uint32_t    ip[INSIDE_IP_NUMS];
            uint32_t    using_node_ip;             //完成该桶的节点信息.如果当前正在回收，存储当前回收的IP，如果完成存储完成的IP...
        };

        //暂时保持MU和SU汇报的心跳信息是相同的...
        struct NodeState
        {
            uint32_t current_bucket_nr;     //该节点当前正在执行的桶编号
            Node_State current_state;         //该节点当前的状态:初始化，执行、暂停、阻塞、上传位图、下载位图、等待token等...
            uint32_t current_finish_items;  //该节点当前完成的infohash数目
            uint32_t current_cpu_rate;      
            uint32_t current_disk_state;
            uint32_t current_net_speed;
            time_t   last_heartbeat_timestamp;
        };

        uint32_t m_agent_index;               //和MPC连接的本次垃圾回收的agent编号
        double   m_error_rate;                //本次垃圾回收的错误率
        uint64_t m_scale;                     //每次垃圾回收的规模，指的是MU中有效的infohash的数目
        uint64_t m_bitmap_size;               //计算得到的本次垃圾回收应该使用的位图大小，对齐之后的字节数
        uint64_t m_hash_numbers;              //本次垃圾回收使用的哈希函数数目
        uint16_t m_block_bits;
        std::vector<int> m_current_hash_ids;       //具体使用的哈希函数的编号，每个哈希函数一个编号,CS只需要保证MU和SU使用的相同的哈希函数

        std::vector<struct Rule> m_all_rules;      //当前保存的所有的规则信息，每一个桶的对应的IP，还包括每个桶的状态...
        uint16_t m_current_token_holders;     //当需要获取上传/下载位图的token的时候，需要保持申请的链表
        uint16_t m_max_token_holders;     //当需要获取上传/下载位图的token的时候，需要保持申请的链表
        std::list<uint32_t> m_token_apply_list;    //申请token的链表，依次发放token...


        RCCheckHeartbeat *m_timer;                    //定时检查所有的心跳连接的状态...
        std::map<uint32_t , RCAgent *> m_all_agents;    //所有与MU或者SU保持的长连接的agent , 也用它查看当前各个节点的状态...
        //所有的节点(正在连接或者出现错误的)都可能出现在该map中
        std::map<uint32_t , struct NodeState*> m_all_heartbeats;    //所有的心跳信息状态..

        //对于那些出现错误的节点，加入到该链表上，所以规则中的所有节点要么在all_agent中(表示当前正常的节点),
        //要么出现在retry_list(表示出现错误的节点，需要定期的重连)
        //!!!对于出现异常的节点(可能是在最初连接失败的或者运行期间停止心跳，或者发送了FIN报文的)，目前要定期不断的重试连接..
        //直到所有的桶的遍历都完成，如果出现最后剩余的桶的所有副本都出现错误的情况,那么就向上层报告错误，暂停回收..
        std::list<uint32_t> m_retry_list;
        //将重试链表分成两个，前一个用来定时器超时之后查看并且重新连接，连接之后将其移动到第二个链表上
        //所以第一个链表是等待重试的链表，第二个是正在重试的链表，如果重试失败需要将第二个链表的该项移动到第一个链表.
        std::list<uint32_t> m_doing_retry_list;

        enum RCState                                 //当前垃圾回收的状态信息,RCManager对象全局存在，通过该变量检查其状态
        {
            START_RUBBISH_RECYCLER = 0 ,             //启动垃圾回收，但是尚未向任何服务器发送目录，初始化状态
            GENERATE_BITMAP ,                        //与MU交互的状态，向MU发送命令生成位图
            FINISH_ALL_BITMAP ,                      //获取了全部桶的位图信息，执行合并操作
            CHECK_INFOHASH ,                         //与SU交互的状态，向SU发送命令检验infohash是否存在
            FINISH_RECYCLER ,                        //完成所有的操作，汇报状态、清理现场
            IDEL_STATE ,                             //空闲状态，表示当前尚未进行垃圾回收
        }m_current_state;
};

#endif
