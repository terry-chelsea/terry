/*
 * =====================================================================================
 *
 *       Filename:  HB_manager.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/05/13 11:43:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "HB_manager.h"
#include "HeartbeatAgent.h"
#include "Log_warp.h"
#include "Lib_tool.h"

HBManager::HBManager()
    :m_mu_last_load() , 
    m_su_last_load() , 
    m_heartbeat_agent_list() , 
    m_closed_agent_num(0)
{

}

HBManager::~HBManager()
{
    m_mu_last_load.clear();
    m_su_last_load.clear();

    HeartbeatAgent *agent = NULL;
    std::list<HeartbeatAgent *>::iterator it;
    for(it = m_heartbeat_agent_list.begin() ; 
            it != m_heartbeat_agent_list.end() ; ++ it)
    {
        agent = *it;
        agent->destory();
        delete agent;
    }
    m_heartbeat_agent_list.clear();

    m_closed_agent_num = 0;
}

void HBManager::add_heartbeat_agent(HeartbeatAgent *hbt)
{
    m_heartbeat_agent_list.push_back(hbt);
}

void HBManager::heartbeat_closed(HeartbeatAgent *hbt)
{
    std::list<HeartbeatAgent *>::iterator it;
    for(it = m_heartbeat_agent_list.begin() ; 
            it != m_heartbeat_agent_list.end() ; ++ it)
    {
        if(*it == hbt)
            break;
    }

    if(it == m_heartbeat_agent_list.end())
    {
        LOG_ERROR("Can not find heartbeatAgent when A heartbeat is closed.Node IP : " + 
                hbt->get_opposite_ip() + " Node Type " + type_str[hbt->get_heartbeat_type()]);
        return ;
    }
    else 
    {
        m_heartbeat_agent_list.erase(it);
    }
    clear_node_info(hbt);

    ++ m_closed_agent_num;
}

void HBManager::clear_node_info(HeartbeatAgent *hbt)
{
    uint32_t ip = 0;
    if(string_to_int_ip(hbt->get_opposite_ip() , ip) < 0)
    {
        LOG_ERROR("HBManager::string ip " + hbt->get_opposite_ip() + " change to int error when clear node info !");
        return ;
    }

    std::map<uint32_t , struct Node_Info>::iterator it;
    it = m_su_last_load.find(ip);
    if(it != m_su_last_load.end())
    {
        m_su_last_load.erase(it);
        return ;
    }

    it = m_mu_last_load.find(ip);
    if(it != m_mu_last_load.end())
    {
        m_mu_last_load.erase(it);
        return ;
    }
    else 
    {
        LOG_EVENT("HBManager::Node is not MU or SU !");
    }
}

void HBManager::init_node_heartbeat_info(RULE_TYPE type , HeartbeatAgent *hbt , uint32_t load)
{
    map<uint32_t , struct Node_Info>::iterator it;
    map<uint32_t , struct Node_Info>::iterator it_end;

    uint32_t ip_int = 0;
    if(string_to_int_ip(hbt->get_opposite_ip() , ip_int) < 0)
    {
        LOG_ERROR("HBManager::string to int ip error when init node info !");
        return ;
    }

    if(MU_RULER == type)
    {
        it = m_mu_last_load.find(ip_int);
        it_end = m_mu_last_load.end();
    }
    else if(SU_RULER == type)
    {
        it = m_su_last_load.find(ip_int);
        it_end = m_su_last_load.end();
    }
    else 
    {
        LOG_ERROR("HBManager::Undefined rule type when init node load info!");
        return ;
    }

    if(it != it_end)
    {
        LOG_ERROR("HBManager::Node IP : " + hbt->get_opposite_ip() + " Existing when init Node Load Infomation!");

        it->second.storage_limit = load;
    }
    else 
    {
        std::cerr<<"add Node IP : "<<hbt->get_opposite_ip()<<" to heartbeat map!"<<std::endl;

        struct Node_Info info;
        info.storage_limit = load;
        
        if(MU_RULER == type)
            m_mu_last_load.insert(make_pair(ip_int , info));
        else 
            m_su_last_load.insert(make_pair(ip_int , info));
    }
}

void HBManager::update_last_bucket_load(RULE_TYPE type , HeartbeatAgent *hbt , const std::vector<struct Bucket_Info> &info_vec)
{
    uint32_t ip_int = 0;
    if(string_to_int_ip(hbt->get_opposite_ip() , ip_int) < 0)
    {
        LOG_ERROR("HBManager::string to int ip error when update node info!");
        return ;
    }

    map<uint32_t , struct Node_Info>::iterator it_find;
    map<uint32_t , struct Node_Info>::iterator it_end;
    if(MU_RULER == type)
    {
        it_find = m_mu_last_load.find(ip_int);
        it_end = m_mu_last_load.end();
    }
    else if(SU_RULER == type)
    {
        it_find = m_su_last_load.find(ip_int);
        it_end = m_su_last_load.end();
    }
    else
    {
        LOG_ERROR("HBManager::Undefined Ruler type when set node info !");
        return ;
    }

    if(it_find == it_end)
    {
        LOG_ERROR("HBManager::Can not find IP : " + hbt->get_opposite_ip() + "when set node info !") ;
        return ;
    }
    
    it_find->second.bucket_list.clear();
    if(MU_RULER == type)
    {
        m_mu_last_load[ip_int].bucket_list = info_vec;
    }
    else 
    {
        m_su_last_load[ip_int].bucket_list = info_vec;
    }
}

int HBManager::check_heartbeat_timeout(int timeout , int &num)
{
    int close_num = 0;
    std::list<HeartbeatAgent *>::iterator it = m_heartbeat_agent_list.begin();
    std::list<HeartbeatAgent *>::iterator it_end = m_heartbeat_agent_list.end();

    HeartbeatAgent *agent = NULL;
    time_t now = time(NULL);
    if((time_t)-1 == now)
    {
        LOG_SYSCALL_ERROR("HBManager::get current time error !");
        return -1;
    }

    while(it != it_end)
    {
        agent = *it;
        if(now - agent->get_last_timestamp() >= timeout)
        {
            it = m_heartbeat_agent_list.erase(it);

            clear_node_info(agent);
            agent->destory();
            delete agent;
            agent = NULL;

            ++ m_closed_agent_num;
        }
        else
            ++ it;
    }

    close_num = m_closed_agent_num;
    num = m_heartbeat_agent_list.size();
    m_closed_agent_num = 0;

    return close_num;
}

void HBManager::display_heartbeat()
{
    std::map<uint32_t , struct Node_Info>::iterator it;
    std::map<uint32_t , struct Node_Info>::iterator it_end;
    uint32_t ip = INVALID_IP;
    std::string ip_str;

    if(!m_su_last_load.empty())
    {
        std::cerr<<"SU Heartbeat Node List : "<<std::endl;
        it = m_su_last_load.begin();
        it_end = m_su_last_load.end();
        for( ; it != it_end ; ++ it)
        {
            ip = it->first;
            int_to_string_ip(ip , ip_str);
            std::cerr<<"SU IP : "<<ip_str<<std::endl;
        }
        std::cerr<<std::endl;
    }


    if(!m_mu_last_load.empty())
    {
        std::cerr<<"MU Heartbeat Node List : "<<std::endl;
        it = m_mu_last_load.begin();
        it_end = m_mu_last_load.end();
        for( ; it != it_end ; ++ it)
        {
            ip = it->first;
            int_to_string_ip(ip , ip_str);
            std::cerr<<"MU IP : "<<ip_str<<std::endl;
        }
        std::cerr<<std::endl;
    }
}

bool HBManager::IP_is_valid_for_migration(RULE_TYPE type , Migration_Info info)
{
    std::map<uint32_t , struct Node_Info>::iterator it;
    std::map<uint32_t , struct Node_Info>::iterator it_end;

    if(MU_RULER == type)
    {
        it = m_mu_last_load.begin();
        it_end = m_mu_last_load.end();
    }
    else
    {
        it = m_su_last_load.begin();
        it_end = m_su_last_load.end();
    }
    bool src_valid = false ;
    bool dst_valid = false ;
    uint32_t ip_temp = INVALID_IP; 

    //遍历整个map查找源地址和目的地址，必须在与CS保持心跳的集群中
    for( ; it != it_end ; ++ it)
    {
        ip_temp = it->first;
        //如果src_valid为假，也就是没有找到src_ip的时候才去比较
        if(!src_valid && (ip_temp == info.src_ip))
        {
            src_valid = true ;
        }
        if(!dst_valid && (ip_temp == info.dest_ip))
        {
            dst_valid = true;
        }

        //如果两个IP都能够找到，说明这次桶迁移是合法的
        if(src_valid && dst_valid)
            break;
    }

    if(it_end == it)
    {
        LOG_ERROR("RulerManager::can not find src IP " 
                + int_to_str(info.src_ip) + " OR dest IP " 
                + int_to_str(info.dest_ip));
        return false ;
    }

    //在查看需要迁移的桶是否存在于源节点上
    struct Node_Info node_info;
    struct Node_Info dest_node_info;
    uint32_t bucket_nr = 0;

    if(MU_RULER == type)
    {
        node_info = m_mu_last_load[info.src_ip];
        dest_node_info = m_mu_last_load[info.dest_ip];
    }
    else 
    {
        node_info = m_su_last_load[info.src_ip];
        dest_node_info = m_su_last_load[info.dest_ip];
    }

    std::vector<Bucket_Info>::iterator it_vec = node_info.bucket_list.begin();
    std::vector<Bucket_Info>::iterator it_vec_end = node_info.bucket_list.end();
    for( ; it_vec != it_vec_end ; ++ it_vec)
    {
        bucket_nr = it_vec->bucket_nr;
        if(bucket_nr == info.bucket_nr)
            break ;
    }
    
    if(it_vec == it_vec_end)
    {
        LOG_ERROR("RulerManager::Bucket number " + int_to_str(info.bucket_nr) + " is not exist in node IP : " + int_to_str(info.src_ip));
        return false ;
    }

    it_vec = dest_node_info.bucket_list.begin();
    it_vec_end = dest_node_info.bucket_list.end();
    for( ; it_vec != it_vec_end ; ++ it_vec)
    {
        bucket_nr = it_vec->bucket_nr;
        if(bucket_nr == info.bucket_nr)
            break ;
    }
    if(it_vec != it_vec_end)
    {
        LOG_ERROR("RulerManager::Bucket number " + int_to_str(info.bucket_nr) + " has existed in node IP : " + int_to_str(info.dest_ip));
        return false ;
    }
    
    return true;
}

