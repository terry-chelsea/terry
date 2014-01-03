#include "HB_manager.h"
#include "rubbish_recycler.h"
#include "SCAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "CS_Epoll.h"
#include "RC_check_heartbeat.h"
#include "CSConfiguration.h"
#include "RC_agent.h"
#include "RC_protocol.h"
#include <cmath>
#include <cassert>

#define PCS 1e-10
#define DEFAULT_BLOCK_BITS 8
#define BASE_TIMER_EXPIRE_TIME  2                  //2S超时一次...
#define HEARTBEAT_COUNTER       4                  //每隔4次定时器超时
#define HEARTBEAT_TIMEOUT       20                 //20s检查一次，如果心跳间隔时间超过20s表示该节点失效了...
#define RC_HEARTBEAT_PORT  14213
#define MAX_HASH_NUMBERS 20
const std::string NFS_path_for_bitmap = "./cstore/bitmap";

#define DEBUG_INFO

//计算出数val的1/n，误差在指定范围内...使用迭代的方法每次逼近结果...
//
static double nsqrt(double val , unsigned int n)
{
    double low = 0 , high = val;
    if(val < 1)
        high = 1;
    double mid = (low + high) / 2;

    if(n == 1 || 1 == val)
        return val;

    double powv = pow(mid , n);
    while(powv - val > PCS || val - powv > PCS)
    {
        if(powv > val)
            high = mid;
        else 
            low = mid;

        if(high - low < PCS)
            break;
        mid = (low + high) / 2;
        powv = pow(mid , n);
    }

//    printf("pow(%lf , %d) = %lf  compare %lf\n" , mid , n , pow(mid , n) , val);
    return mid;
}

//根据bloom filter算法的数学模型在给定元素集合大小和错误率时候计算出最佳的位图大小和对应的
//哈希函数的数目
static double cal_bitmap_size(unsigned long long set_size , double err_rate , int &hash)
{
    double min = set_size * 1000;
    double m = 0;
    
    int i = 0;
    for(i = 2 ; i <= MAX_HASH_NUMBERS ; ++ i)
    {
        double tmp = 1 - nsqrt(err_rate , i);
        m = (-1) * i * (set_size + 0.5) / log(tmp) + 1;
        if(m < min)
        {
            min = m;
            hash = i;
        }
    }

    return min;
}

//maybe you should get node rule from network...
int RCManager::get_all_rules(RULE_TYPE type)
{
    RulerManager *handler = RulerManager::getInstance();
    int buckets = handler->get_bucket_number(type);
     //将获得的全局的规则保存在RCManager中
    for(int i = 0 ; i < buckets ; ++ i)
    {
        struct HashRule   one_line;
        if(handler->get_hash_rule(i , MU_RULER , one_line) < 0)
        {
            LOG_ERROR("Get MU Bucket rule failed ...");
            return -1;
        }

        struct Rule inside_rule;
        inside_rule.status = NOT_START;
        for(int j = 0 ; j < INSIDE_IP_NUMS ; ++ j)
        {
            (inside_rule.ip)[j] = (one_line.ip)[j];
        }

        inside_rule.bucket_nr = i;
        inside_rule.using_node_ip = INVALID_IP;
        m_all_rules.push_back(inside_rule);
    }

    uint32_t temp = 0;
     //查找所有的节点,使用map可以保证每一个IP只插入一次
    for(int i = 0 ; i < buckets ; ++ i)
    {
        struct Rule one_line = m_all_rules[i];
        for(int j = 0 ; j < INSIDE_IP_NUMS ; ++ j)
        {
            temp = (one_line.ip)[j];
             //insert it to set no matter if it exist...
            if(temp != INVALID_IP)
                //奇葩!!为什么使用NULL 就报错...
                m_all_agents.insert(make_pair(temp , (RCAgent *)NULL));
        }
    }
    
#ifdef DEBUG_INFO
    std::string ip;
    std::string node_type = (type == MU_RULER ? "MU" : "SU");
    std::cerr<<"Current "<< node_type << " have "<< buckets <<" buckets : "<<std::endl;
    for(int i = 0 ; i < buckets ; ++ i)
    {
        std::cerr<<"Bucket NO."<< i <<" IP list : ";
        for(int j = 0 ; j < INSIDE_IP_NUMS ; ++ j)
        {
            int_to_string_ip(m_all_rules[i].ip[j] , ip);
            std::cerr<< ip << " , "<<std::endl;
        }
        std::cerr<<std::endl;
    }

    std::cerr<<"There are "<< m_all_agents.size() << node_type << " nodes to connect..."<<std::endl;
#endif

    return 0;
}

void RCManager::check_heartbeat_state()
{
    std::cerr<<"RC check timer expire ..."<<std::endl;
    if(m_current_state != GENERATE_BITMAP && m_current_state != CHECK_INFOHASH)
        return ;

    time_t now = time(NULL);
    std::map<uint32_t , struct NodeState*>::iterator it = m_all_heartbeats.begin();
    for( ; it != m_all_heartbeats.end() ; ++ it)
    {
        if(now - it->second->last_heartbeat_timestamp > HEARTBEAT_TIMEOUT)
        {
            std::map<uint32_t , RCAgent *>::iterator it_agent = m_all_agents.find(it->first);
            if(it_agent != m_all_agents.end())
            {
                agent_run_error(it_agent->second);
            }
            else 
                LOG_ERROR("Agent for node " + it_agent->second->getIP() + " not exist ...");
        }
    }

    std::list<uint32_t>::iterator retry_it = m_retry_list.begin();
    while(retry_it != m_retry_list.end())
    {
        std::map<uint32_t , RCAgent *>::iterator it_cur = m_all_agents.find(*retry_it);
        RCAgent *agent = new RCAgent(CSEpoll::getInstance());
        if(NULL == agent)
        {
            LOG_ERROR("Create agent to failed !");
            continue ;
        }
        if(it_cur != m_all_agents.end())
        {
            if(agent->init(it_cur->first , RC_HEARTBEAT_PORT , 
                    std::tr1::bind(&RCManager::agent_callback , this , 
                        std::tr1::placeholders::_1 , 
                        std::tr1::placeholders::_2)) < 0)
            {
                LOG_ERROR("Connect to node " + agent->getIP() + " failed");
            }
            else 
            {
                retry_it = m_retry_list.erase(retry_it);
                it_cur->second = agent;
            }
        }
    }
}


int RCManager::connect_all_node_start_rc(const std::string &type)
{
    RCCheckHeartbeat *timer = new RCCheckHeartbeat(CSEpoll::getInstance() , BASE_TIMER_EXPIRE_TIME);
    if(NULL == timer)
    {
        LOG_ERROR("Create timer for " + type + " failed ...");
        return -1;
    }

    timer->set_callback(std::tr1::bind(&RCManager::check_heartbeat_state , this));
    if(timer->init() < 0)
    {
        LOG_ERROR("Create timer failed ....");
        return -1;
    }

    //为每一个节点创建一个agent用于保持CS与节点之间的长连接...
    std::map<uint32_t , RCAgent *>::iterator it_cur = m_all_agents.begin();
    for( ; it_cur != m_all_agents.end() ; ++ it_cur)
    {
        RCAgent *agent = new RCAgent(CSEpoll::getInstance());
        if(NULL == agent)
        {
            LOG_ERROR("Create agent to for " + type + " failed !");
            delete timer;
            return -1;
        }
        it_cur->second = agent;
    }

    //再为每一个创建的agent连接对应的节点，如果连接失败，将该节点剔除
    it_cur = m_all_agents.begin();
    while(it_cur != m_all_agents.end())
    {
        RCAgent *agent = it_cur->second;
        if(agent->init(it_cur->first , RC_HEARTBEAT_PORT , 
                    std::tr1::bind(&RCManager::agent_callback , this , 
                        std::tr1::placeholders::_1 , 
                        std::tr1::placeholders::_2)) < 0)
        {
            std::string ip;
            int_to_string_ip(it_cur->first , ip);
            LOG_ERROR("Connect to " + type + " node " + ip + " failed");
            m_retry_list.push_back(it_cur->first);
        }

        ++ it_cur;
    }
    m_timer = timer;

    return 0;
}

void RCManager::init_paramaters()
{
    int hash_numbers;
    double bits = cal_bitmap_size(m_scale , m_error_rate , hash_numbers);
    uint64_t ubits = (uint64_t)bits;
    uint32_t blocks = 2 << DEFAULT_BLOCK_BITS;
    m_bitmap_size = ubits / 8;
    m_bitmap_size = (m_bitmap_size + (blocks << 2) - 1) & ~((blocks << 2) - 1);
    
    m_hash_numbers = hash_numbers;
    m_block_bits = DEFAULT_BLOCK_BITS;

#ifdef DEBUG_INFO
    std::cerr<<"Start Rubbish Recycler ..."<<std::endl;
    std::cerr<<"Agent index : " << m_agent_index << " and scale blocks : " << m_scale <<" error rate : "<< m_error_rate <<std::endl;
    std::cerr<<"Need bitmap "<< m_bitmap_size <<" bytes and hash functions : "<< m_hash_numbers <<std::endl;
#endif
}

void RCManager::choice_hash_functions()
{
    //get the hash functions which MU and SU should use...
    struct timeval now;
    gettimeofday(&now , NULL);
    srand(((uint32_t)(now.tv_sec)) ^ ((uint32_t)(now.tv_usec)));
    //每次随机选择使用的哈希函数的编号...
    for(uint32_t i = 0 ; i < m_hash_numbers ; ++ i)
    {
        int id = rand() % MAX_HASH_NUMBERS;
        uint32_t j = 0;
        for(j = 0 ; j < i ; ++ j)
        {
            if(m_current_hash_ids[j] == id)
                break;
        }
        if(j == i)
            m_current_hash_ids.push_back(id);
        else 
            -- i;
    }

#ifdef DEBUG_INFO

    std::cerr<<"This time choice " << m_hash_numbers <<" hash functions(id) : "<<std::endl;
    for(int i = 0 ; i < m_hash_numbers ; ++ i)
        std::cerr<<m_current_hash_ids[i]<<"  ";
    std::cerr<<std::endl;

#endif

}

int RCManager::start_do_recylcer(uint32_t agent_index , unsigned long long scale , double error_rate)
{
    //第一步、根据有效集合的大小(这里MPC给定的是一个参考值)和执行的错误率，计算出对应的
    //最佳的大小和哈希函数的数目...
    m_current_state = START_RUBBISH_RECYCLER;
    m_scale = scale;
    m_error_rate = error_rate;
    m_agent_index = agent_index;

    init_paramaters();
    choice_hash_functions();
    if(get_all_rules(MU_RULER) < 0)
    {
        LOG_ERROR("Get all MU rule from CS failed ...");
        m_current_state = IDEL_STATE;
        return -1;
    }

    //中间需要首先通知所有的MU和SU本次垃圾回收的一些配置信息：
    //对于所有的节点都需要知道需要哪些哈希函数进行计算和验证。

    //第二步，创建一些agent向MU发起连接...
    if(connect_all_node_start_rc("MU") < 0)
    {
        LOG_ERROR("start all MU to start rubbish recycler failed ...");
        return -1;
    }
    m_current_state = GENERATE_BITMAP;
    //第三步，监视MU的状态，等待MU操作的完成...
    
    //第七步，统计删除信息，查看MPC是否关闭，释放对应的资源...

    return 0;
}

//第四步，等待所有的bucket都完成，合并全部MU得到的位图...
int RCManager::start_merge_all_bitmap()
{
    int flag = 0;
    LOG_EVENT("Start merge all bitmaps ...");
    if(m_current_state == GENERATE_BITMAP)
        m_current_state = FINISH_ALL_BITMAP;
    else 
    {
        flag = 1;
        m_current_state = FINISH_RECYCLER;
    }
    
    m_all_rules.clear();
    m_current_token_holders = 0;
    m_token_apply_list.clear();
    std::map<uint32_t , RCAgent *>::iterator it = m_all_agents.begin();

    for( ; it != m_all_agents.end() ; ++ it)
    {
        RCAgent *agent = it->second;
        agent->delete_agent();
        delete agent;
    }
    m_all_agents.clear();

    std::map<uint32_t , struct NodeState *>::iterator it_hb = m_all_heartbeats.begin();
    for( ; it_hb != m_all_heartbeats.end() ; ++ it_hb)
        delete it_hb->second;

    m_all_heartbeats.clear();
    m_retry_list.clear();

    if(flag)
    {
        LOG_EVENT("All rubbish recycler finish !!!");
        delete m_timer;
        return 0;
    }
    
    //第五步，创建一些agent向SU发起连接...
    if(get_all_rules(SU_RULER) < 0)
    {
        LOG_ERROR("Get all SU rule from CS failed ...");
        m_current_state = IDEL_STATE;
        return -1;
    }

    if(connect_all_node_start_rc("SU") < 0)
    {
        LOG_ERROR("start all SU to start rubbish recycler failed ...");
        return -1;
    }
    m_current_state = CHECK_INFOHASH;
    //第六步，监视这些鱼SU保持通信的连接的状态，并且处理删除操作...
    return 0;
}

//当agent连接失败的时候，需要将规则中所有出现这个agent的项加入到重试链表中，定期重试(防止节点短暂的暂停服务并重新启动)
void RCManager::agent_connect_error(RCAgent *agent)
{
    uint32_t ip = agent->agent_ip();
    m_retry_list.push_back(ip);                    //加入到重试队列上等待被重试..
//    m_all_agents[ip] = NULL;                       //置为NULL表示当前节点不可用..
}

//当agent运行时出现错误，例如对端发送FIN报文，或者socket出现错误，这时应该将其加入到重试链表上...
void RCManager::agent_run_error(RCAgent *agent)
{
    uint32_t ip = agent->agent_ip();
    std::map<uint32_t , struct NodeState *>::iterator node = m_all_heartbeats.find(ip);
    //如果这个节点没有任何的心跳信息，说明没有正在回收的桶，因此需要在发送回收命令之前添加心跳信息...
    //否则找到该节点正在进行的桶，然后更改桶的状态...
    if(node != m_all_heartbeats.end())
    {
        if(node->second->current_state != INITING)
        {
            uint32_t bucket = node->second->current_bucket_nr;
            assert(bucket == m_all_rules[bucket].bucket_nr);
            assert(ip == m_all_rules[bucket].using_node_ip);
            assert(WORKING == m_all_rules[bucket].status);

            m_all_rules[bucket].status = NOT_START;
            m_all_rules[bucket].using_node_ip = 0;
        }
        m_all_heartbeats.erase(node);
    }
    
    m_retry_list.push_back(ip);       //加入到重试队列上等待被重试..
    m_all_agents[ip] = NULL;          //置为NULL表示当前节点不可用..
}

//当连接成功之后需要向节点发送控制信息，并且添加心跳信息...
//对于MU节点，需要发送位图大小，分块大小(模数)，哈希函数，需要上传的NFS路径，是否压缩，是否需要申请令牌环...
int RCManager::send_first_command(RCAgent *agent)
{
    uint32_t ip = agent->agent_ip();

    struct NodeState *node = new struct NodeState;
    node->current_bucket_nr = 0;
    node->current_state = INITING;
    node->current_finish_items = 0;
    node->current_cpu_rate = 0;
    node->current_disk_state = 0;
    node->current_net_speed = 0;
    node->last_heartbeat_timestamp = time(NULL);

    m_all_heartbeats.insert(make_pair(ip , node));

    NodeInitInfo info;
    memset(&info , 0 , sizeof(info));
    info.bitmap_size = m_bitmap_size;
    info.block_bits = m_block_bits;
    info.hash_funcs_num = m_hash_numbers;
    info.compress_flag = 1;                         //always compress...
    info.token_flag = 1;                            //always apply for token ...
    info.heartbeat_interval = HEARTBEAT_TIMEOUT >> 1;
    
    int path_length = NFS_path_for_bitmap.size();
    memcpy(info.nfs_path , NFS_path_for_bitmap.c_str() , path_length);
    info.nfs_path[path_length] = '\0';

    for(uint32_t i = 0 ; i < m_hash_numbers ; ++ i)
        info.hash[i] = m_current_hash_ids[i];

    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    header.cmd = RC_CS_NODE_INIT_MSG;
    header.length  = sizeof(info);
    if(agent->write_data(&header , sizeof(header)) != 0 || 
            agent->write_data(&info , sizeof(info)))
    {
        LOG_ERROR("Write data to node " + agent->getIP() + " failed !");
        return -1;
    }

    return 0;
}

int RCManager::find_next_bucket(uint32_t ip)
{
    uint32_t finished = 0;
    vector<struct Rule>::iterator it_cur = m_all_rules.begin();
    for( ; it_cur != m_all_rules.end() ; ++ it_cur)
    {
        if(it_cur->status == FINISHED)
        {
            ++ finished;
            continue ;
        }
        if(it_cur->status == NOT_START)
        {
            int i = 0;
            for(i = 0 ; i < MAX_IP_NUM ; ++ i)
            {
                if(it_cur->ip[i] == ip)
                    break;
            }
            if(i < MAX_IP_NUM)
                break;
        }
    }
    if(finished == m_all_rules.size())
        return -2;

    if(it_cur != m_all_rules.end())
        return it_cur->bucket_nr;
    else 
        return -1;
}

int RCManager::start_bucket(RCAgent *agent , int bucket_nr)
{
    MsgHeader head;
    memset(&head , 0 , sizeof(head));
    BucketInfo info;
    info.bucket_nr = bucket_nr;
    head.cmd = RC_CS_NODE_START_BUCKET_MSG;
    head.length = sizeof(Bucket_Info);
    head.error = 0;

    if(agent->write_data(&head , sizeof(head)) < 0 || 
            agent->write_data(&info , sizeof(info)) < 0)
    {
        LOG_ERROR("Sending start bucket " + int_to_str(bucket_nr) + " message error to node " + agent->getIP());
        return -1;
    }

    return 0;
}

int RCManager::deal_with_data(void *data , RCAgent *agent)
{
    InReq *req = (InReq *)data;
    
    MsgHeader *header = &(req->m_msgHeader);
    uint32_t cmd = header->cmd;
    uint32_t err = header->error;
    MsgHeader head;
    memset(&head , 0 , sizeof(head));

    HeartbeartInfo *info = NULL;
    struct NodeState *state = NULL;
    struct Rule *rule = NULL;
    uint32_t ip = INVALID_IP;

    int bucket_nr = 0;
    switch(cmd)
    {
        case RC_CS_NODE_INIT_MSG_ACK : 
            if(err != 0)
            { 
                LOG_ERROR("Node " + agent->getIP() + " Start error ...");
                send_first_command(agent);                //retry when error happened...
            }
            else 
            {
                LOG_EVENT("Node " + agent->getIP() + " start success , waiting for bucket commands...");
            }
            break;

        case RC_CS_NODE_HEARTBEAT_MSG : 
            info = (HeartbeartInfo *)(req->ioBuf);
            ip = agent->agent_ip();
            state = m_all_heartbeats[ip];
            assert(state->current_bucket_nr == info->bucket_nr);
            state->current_state = (RCManager::Node_State)(info->state);
            state->current_finish_items = info->finish_items;
            state->current_cpu_rate = info->cpu_rate;
            state->current_disk_state = info->disk_state;
            state->current_net_speed = info->net_speed;
            state->last_heartbeat_timestamp = time(NULL);

            head.cmd = RC_CS_NODE_HEARTBEAT_MSG_ACK;
            head.length = 0;
            head.error = 0;

            if(agent->write_data(&head , sizeof(head)) != 0)
            {
                LOG_ERROR("Write heartbeat ack message error to Node " + agent->getIP());
                return -1;
            }

            LOG_EVENT("Heartbeat recieved from Node " + agent->getIP() + ", Current bucket " + int_to_str(info->bucket_nr));
            break;

        case RC_CS_NODE_STOP_RECYCLER_MSG_ACK :
            if(err != 0)
            {
                LOG_ERROR("Node " + agent->getIP() + " stop recycler failed ...");
            }
            else
                LOG_EVENT("Node " + agent->getIP() + " stop recycler success ...");
            break;

        case RC_CS_NODE_CONTINUE_RECYCLER_MSG_ACK : 
            LOG_EVENT("Node " + agent->getIP() + " continue recycler result : " + int_to_str(err));
            break;

        case RC_CS_NODE_CANCEL_RECYCLER_MSG_ACK :
            LOG_EVENT("Node " + agent->getIP() + " cancel recycler result : " + int_to_str(err));
            break;

        case RC_CS_NODE_START_BUCKET_MSG_ACK :
            bucket_nr = ((BucketInfo *)(req->ioBuf))->bucket_nr;
            if(0 == err)
            {
                LOG_EVENT("Start recycler bucket " + int_to_str(bucket_nr) + " success ...");
                m_all_rules[bucket_nr].status = WORKING;
                m_all_rules[bucket_nr].bucket_nr = bucket_nr;
                m_all_rules[bucket_nr].using_node_ip = agent->agent_ip();
            }
            else 
            {
                LOG_EVENT("Start recycler bucket " + int_to_str(bucket_nr) + " failed : " + int_to_str(err));
                //waiting for the idle node restart this bucket ...
                
                bucket_nr = find_next_bucket(agent->agent_ip());
                if(bucket_nr > 0 && start_bucket(agent , bucket_nr))
                {
                    LOG_ERROR("Sending start bucket " + int_to_str(bucket_nr) + " to " + agent->getIP() + " failed ...");
                    return -1;
                }
                else if(bucket_nr < 0)
                    LOG_EVENT("No bucket selected for node " + agent->getIP());

            }
            break;

        case RC_CS_NODE_CANCEL_BUCKET_MSG_ACK : 
            LOG_EVENT("Node " + agent->getIP() + " continue recycler result : " + int_to_str(err));
            break;

        case RC_NODE_CS_GET_TOKEN_MSG :
            head.cmd = RC_NODE_CS_GET_TOKEN_MSG_ACK;
            head.length = 0;
            if(m_current_token_holders == m_max_token_holders)
            {
                head.error = 1;
                m_token_apply_list.push_back(agent->agent_ip());
                LOG_EVENT("Add this IP " + int_to_str(agent->agent_ip()) + " to waiting token list ...");
            }
            else 
            {
                head.error = 0;
                m_current_token_holders += 1;
            }
            if(agent->write_data(&head , sizeof(head)) < 0)
            {
                LOG_ERROR("Write get token ack to node " + agent->getIP() + " failed ...");
                return -1;
            }

            break;

        case RC_MU_CS_UPLOAD_BITMAP_MSG : 
        case RC_SU_CS_DOWNLOAD_BITMAP_MSG :
            head.cmd = RC_MU_CS_UPLOAD_BITMAP_MSG_ACK;
            if(RC_SU_CS_DOWNLOAD_BITMAP_MSG == cmd)
                head.cmd = RC_SU_CS_DOWNLOAD_BITMAP_MSG_ACK;
            head.length = 0;
            head.error = 0;
            if(agent->write_data(&head , sizeof(head)))
            {
                LOG_ERROR("Sending upload ack to node " + agent->getIP() + " failed ...");
                return -1;
            }
            if(m_token_apply_list.size() > 0)
            {
                ip = m_token_apply_list.front();
                m_token_apply_list.pop_front();
                std::map<uint32_t , RCAgent *>::iterator it = m_all_agents.find(ip) ;
                if(m_all_agents.end() == it)
                {
                    LOG_ERROR("Error happened , can not find this IP " + int_to_str(ip) + " in waiting token list ...");
                }
                else 
                {
                    head.cmd = RC_NODE_CS_GET_TOKEN_MSG_ACK;
                    head.length = 0;
                    head.error = 0;
                    RCAgent *next = it->second;
                    if(next->write_data(&head , sizeof(head)) < 0) 
                    {
                        LOG_ERROR("Send to node " + next->getIP() + " Got token failed ...");
                        agent_run_error(next);
                    }
                }
            }
            break;

        case NODE_CS_BUCKET_FINISH_MSG :
            bucket_nr = ((BucketInfo *)(req->ioBuf))->bucket_nr;
            rule = &m_all_rules[bucket_nr];
            ip = agent->agent_ip();
            if(err != 0)
            {
                LOG_EVENT("Node " + agent->getIP() + " recycler bucket " + int_to_str(bucket_nr) + " failed , error code : " + int_to_str(err));
                rule->status = NOT_START;
                rule->using_node_ip = 0;
                int i = 0;
                for(i = 0 ; i < INSIDE_IP_NUMS && rule->ip[i] != ip ; ++ i);
                assert(i != INSIDE_IP_NUMS);
                rule->ip[i] = 0;
            }
            else 
            {
                LOG_EVENT("Node " + agent->getIP() + " recycler bucket " + int_to_str(bucket_nr) + " success ...");
                rule->status = FINISHED;
                rule->using_node_ip = 0;
            }
            
            bucket_nr = find_next_bucket(ip);
            if(-2 == bucket_nr)
            {
                LOG_EVENT("All bucket finished ...");
                start_merge_all_bitmap();
            }
            else if (bucket_nr < 0)
                LOG_EVENT("No bucket selected for node " + agent->getIP());
            else if(start_bucket(agent , bucket_nr) < 0)
            {
                LOG_ERROR("Start recycler bucket " + int_to_str(bucket_nr) + " at node " + agent->getIP() + " failed ...");
                return -1;
            }
            break;

        default : 
            LOG_ERROR("Undefined command " + int_to_str(cmd) + " From Node " + agent->getIP());
            break;
    }

    return 0;
}

//该函数负责agent所有的回调操作，第一个参数表示回调事件类型，第二个参数为对应的参数，该参数是对应的agent对象的地址
void RCManager::agent_callback(RC_TYPE ret , void *para)
{
    RCAgent *agent = (RCAgent *)para;
    switch(ret)
    {
        case RC_CONNECT_FAILED : 
            //连接出错，将该节点移除
            LOG_ERROR("Connect to node " + agent->getIP() + " failed ");
            agent_connect_error(agent);
            break;

        case RC_CONNECT_SUCCESS :
            //连接成功，向该节点发送启动命令
            LOG_ERROR("Connect to node " + agent->getIP() + " successfully ");
            if(send_first_command(agent) < 0)
            {
                LOG_ERROR("Send first command to node " + agent->getIP() + " failed ");
                agent->recycler();
                delete agent;
            }
            break;

        case RC_SUCCESSFULLY : 
            //成功收到对端的报文，根据命令执行具体操作
            LOG_EVENT("read data from node " + agent->getIP() + " successfully ");
            if(deal_with_data(agent->get_data() , agent) < 0)
            {
                LOG_ERROR("Deal with read data from " + agent->getIP() + " failed ...");
                agent_run_error(agent);
            }
            break;

        case RC_WRITE_FAILED: 
            //写数据出现错误，将该节点移除
            LOG_ERROR("Write data to node " + agent->getIP() + " error ");
            agent_run_error(agent);
            break;

        case RC_UNKNOW_ERROR : 
            //agent出现未知的错误，说明该连接出现问题，可以重新尝试连接该节点，或者将该节点移除
            LOG_ERROR("Agent connecting to node " + agent->getIP() + " running error ");
            agent_run_error(agent);
            break;

        default :
            LOG_ERROR("undefined callback command ...");
            if(agent != NULL)
                agent_run_error(agent);
            break;
    }
}
