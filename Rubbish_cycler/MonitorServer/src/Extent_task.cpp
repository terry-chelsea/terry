/*
 * =====================================================================================
 *
 *       Filename:  Extent_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/16/12 01:40:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Extent_task.h"
#include "CSTCPAgent.h"
#include "AgentRegister.h"
#include "CSConfiguration.h"
#include "CS_Epoll.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "Task_manager.h"
#include "Extent_work_item.h"
#include "Dispatcher.h"
#include "HB_manager.h"

#include "../common/protocol/protocol.h"

ExtentTask::ExtentTask(uint32_t agent_index , RULE_TYPE type , Extent_Info info)
    :Task(agent_index) , 
    m_type(type) , 
    m_ext_info(info) , 
//    m_notice_task(NULL) , 
    m_state(FREE) , 
    m_extent_ret(false) , 
    m_item(NULL)
{
}

ExtentTask::~ExtentTask()
{
    std::list<uint32_t>::iterator it = m_extent_agent_list.begin();
    std::list<uint32_t>::iterator it_end = m_extent_agent_list.end();

    uint32_t index = 0;
    for( ; it != it_end ; ++ it)
    {
        index = *it;
        AgentRegister::getInstance()->delete_SC_agent(index);
    }
    
    m_extent_agent_list.clear();
    m_state = FREE ;
}

//判断是否可以执行桶迁移操作
bool ExtentTask::can_exec_migration(uint32_t src , uint32_t dest)
{
    //第一轮操作尚未完成，没有得到全部的回复
    if(EXTENTING == m_state) 
        return false ;

    uint32_t agent_index = 0;
    uint32_t node_ip = INVALID_IP;
    std::list<uint32_t>::iterator it = m_retry_agent_list.begin();
    std::list<uint32_t>::iterator it_end = m_retry_agent_list.end();

    std::map<uint32_t , uint32_t>::iterator it_find;
    for( ; it != it_end ; ++ it)
    {
        agent_index = *it;
        it_find = m_index_ip_map.find(agent_index);
        if(it_find == m_index_ip_map.end())
        {
            LOG_ERROR("ExtentTask::Can not find agent index " + int_to_str(agent_index)
                    + " in index to IP map !");
            continue ;
        }
        else 
        {
            node_ip = it_find->second;
            //源和目的都不能在重试
            if(src == node_ip || dest == node_ip)
                return false;
        }
    }

    return true;
}

int ExtentTask::init(std::tr1::function<void (bool , RULE_TYPE , struct Extent_Info)> ret_callback)
{
    m_callback_func = ret_callback;
    m_state = EXTENTING;

    //首先修改规则,创建Task完成
    //按照桶迁移将所有的桶进行扩展，修改数据库
    //修改完成数据库之后再修改内存
    //设置的回调函数done_modify_rule
    
    
    //修改数据库规则
    WorkItem *item = new ExtentWorkItem(m_type , m_ext_info);

    item->init(std::tr1::bind(&ExtentTask::done_modify_rule , this , 
                std::tr1::placeholders::_1));

    Dispatcher::getInstance()->send_request(item) ;
    m_item = item;

    std::cerr<<"--------------info Extention--------------"<<std::endl;
    std::cerr<<"send modify rule work item to thread polll !"<<std::endl;
    
    return 0;
}

void ExtentTask::done_modify_rule(bool ret)
{

    std::cerr<<"----------------extention info----------------"<<std::endl;
    std::cerr<<"modify rule workitem reply and work "<<(ret ? "Successfully !" : "failed !")<<std::endl;

    //如果更新失败，回调，
    if(!ret)
    {
        do_last_work(false , "Update database rule error !");
        return ;
    }

    //修改内存中的规则表
    RulerManager::getInstance()->new_extent_order(m_type);

    //如果更新规则成功，向每一个节点发送请求
    uint32_t node_ip = INVALID_IP;
    uint32_t agent_index = 0;

    std::map<uint32_t , struct Node_Info>::iterator it;
    std::map<uint32_t , struct Node_Info>::iterator it_end;

    //根据当前与CS保持心跳的节点进行,每个与CS保持心跳连接的MU或者SU节点
    //都会将最新更新的信息保存在last_load中，所以只需要遍历这个map
    it = HBManager::getInstance()->map_begin(m_type);
    it_end = HBManager::getInstance()->map_end(m_type);

    for( ; it != it_end ; ++ it)
    {
        node_ip = it->first;
        //如果创建Agent并执行init失败，忽略这个节点，仅仅将成功的加入到list中
        if(create_agent_and_init(node_ip , agent_index) < 0)
        {
            LOG_ERROR("ExtentTask::create and init agent error !");
            continue ;
        }

        //只管理发送命令的节点的Agent句柄
        m_extent_agent_list.push_back(agent_index);
    }
}

//第二个参数用来返回创建Agent的index，用于加入队列
int ExtentTask::create_agent_and_init(uint32_t node_ip , uint32_t &index_ret)
{
    std::string load_string;
    std::string out_data;
    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    uint32_t current_mod = 0;
    SCAgent *agent = NULL;
    uint16_t node_port = 0;
    uint32_t agent_index = 0;

    if(MU_RULER == m_type)
    {
        header.cmd = MSG_CS_MU_EXTENT_BUCKET;
        node_port = CSConfiguration::getInstance()->mu_port;
    }
    else
    {
        header.cmd = MSG_CS_SU_EXTENT_BUCKET;
        node_port = CSConfiguration::getInstance()->su_port;
    }

    cstore::pb_MSG_CS_MU_EXTENT_BUCKET pb_mu_info;
    cstore::pb_MSG_CS_SU_EXTENT_BUCKET pb_su_info;

    current_mod = m_ext_info.new_mod;
    agent = Task::create_new_agent(node_ip , node_port);
    if(NULL == agent)
    {
        LOG_ERROR("ExtentTask::create new agent error ! node IP : " 
                + int_to_str(node_ip));
        return -1;
    }
        
    if(MU_RULER == m_type)
    {
        pb_mu_info.set_new_mod(current_mod);
        if((!pb_mu_info.IsInitialized()) || (!pb_mu_info.SerializeToString(&load_string)))
        {
            LOG_ERROR("ExtentTask::protobuf MU extent info Initialized error ! Node IP : " 
                    + int_to_str(node_ip));
            return -1;
        }
    }
    else
    {
        pb_su_info.set_new_mod_nr(current_mod);
        if((!pb_su_info.IsInitialized()) || (!pb_su_info.SerializeToString(&load_string)))
        {
            LOG_ERROR("ExtentTask::protobuf SU extent info Initialized error ! Node IP : " 
                    + int_to_str(node_ip));
            return -1;
        }
    }

    header.length = load_string.size();
    agent->do_package(header , load_string , out_data);

    agent_index = agent->get_agent_index();
    //把Agent的index作为最后一个参数传递，回调的时候进行查看
    if(agent->init(std::tr1::bind(&ExtentTask::done_extent_ack , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3) , out_data ,
                (void *)agent_index) < 0)
    {
        LOG_ERROR("ExtentTask::agent init error ! Node IP : " 
                + int_to_str(node_ip));

        AgentRegister::getInstance()->delete_SC_agent(agent_index);
        return -1;
    }
    
    //生成index和对端IP的映射
    m_index_ip_map.insert(pair<uint32_t , uint32_t>(agent_index , node_ip));
    index_ret = agent_index;
    std::cerr<<"create agent to ip : "<<node_ip<<" agent index : "<<agent_index<<" successfully !"<<std::endl;
    return 0;
}

void ExtentTask::done_extent_ack(void *req , RET_TYPE type , void *data)
{
    uint32_t agent_index = 0;
    //如果data为NULL，说明出现错误直接返回,因为无法找到对于的agent
    if(NULL == data)
    {
        LOG_FATAL("ExtentTask::NO data info after node extent !");
        return ;
    }
    else
    {
        agent_index = (uint64_t)data;
    }

    std::cerr<<"------------------extention info --------------"<<std::endl;
    std::cerr<<"agent index "<<agent_index<<" retrun back !"<<std::endl;

    InReq *back_req = NULL;
    MsgHeader header;
    memset(&header , 0 , sizeof(header));

    //需要特别注意UNKOWN_ERROR，这说明对端主动关闭了连接，认为扩展操作执行成功
    if(parse_callback_ret(req , type) < 0)
    {
        goto RETRY;
    }
    if(UNKOWN_ERROR == type)
    {
        LOG_ERROR("ExtentTask::the opposite close this agent !");
        goto OPPO_FIN;
    }
    back_req = (InReq *)req;

    header = back_req->m_msgHeader;
    if(MU_RULER == m_type)
    {
        if(MSG_CS_MU_EXTENT_BUCKET_ACK != header.cmd)
        {
            LOG_ERROR("ExtentTask::Undefined MU extent ack cmd : " + int_to_str(header.cmd));
            goto RETRY ;
        }
        else if(MU_OK != header.error)
        {
            LOG_ERROR("ExtentTask::MU Node exec extent error ! Agent Index : " 
                    + int_to_str(agent_index));
            goto RETRY ;
        }
    }
    else
    {
        if(MSG_CS_SU_EXTENT_BUCKET_ACK != header.cmd)
        {
            LOG_ERROR("ExtentTask::Undefined SU extent ack cmd : " + int_to_str(header.cmd));
            goto RETRY ;
        }
        else if(SU_OK != header.error)
        {
            LOG_ERROR("ExtentTask::SU Node exec extent error ! Agent Index : " 
                    + int_to_str(agent_index));

            std::cerr<<"su return error code : "<<header.error<<std::endl;
            goto RETRY ;
        }
    }

    //这里说明执行成功了，将这个Agent析构掉
    //如果对端在扩展的时候主动关闭了，认为它已经掉线，将它对应的Agent认为操作成功不再重试！
    //但是它对应的Agent已经被回收（基类），因此不需要再回收，第二个参数表示需要不需要回收
OPPO_FIN:
    agent_exec_extent_back(agent_index , UNKOWN_ERROR != type);
    return ;

RETRY :
    retry_do_extent(agent_index);

}

//是这样安排的，无论对于成功的节点还是失败的节点，第一次回调都会讲它从extent_list
//上移除，（！！)对于成功的节点，接下来就要析构它的Agent了
//但是对于失败的节点，需要进行重试，首先查看对于的Agent是否存在
//如果不存在了重新创建Agnet然后加入retry_list中
//如果存在就再次向节点进行桶扩展请求的发送，并将它加入到retry_list中
//下次回调的时候执行相同的流程，这时候根据成功或者失败，还是会首先移除该index
//这时候如果不存在，说明这是重试的请求的回调。
//当然还可以通过传递给Agent一个特殊的结构作为私有参数，标志着是第一次还是重试

bool ExtentTask::remove_agent_from_extent_list(uint32_t agent_index)
{
    if(m_extent_agent_list.empty())
        return false;

    std::list<uint32_t>::iterator it = m_extent_agent_list.begin();
    std::list<uint32_t>::iterator it_end = m_extent_agent_list.end();

    for( ; it != it_end ; ++ it)
    {
        if(agent_index == *it)
            break ;
    }
    if(it == it_end)
    {
        LOG_EVENT("ExtentTask::can not find agent from list ! retry state...");
        return false;
    }
    else 
    {
        m_extent_agent_list.erase(it);
    }
    return true;
}

bool ExtentTask::remove_agent_from_retry_list(uint32_t agent_index)
{
    if(m_retry_agent_list.empty())
        return false;
    
    std::list<uint32_t>::iterator it = m_retry_agent_list.begin();
    std::list<uint32_t>::iterator it_end = m_retry_agent_list.end();

    for( ; it != it_end ; ++ it)
    {
        if(agent_index == *it)
            break ;
    }
    if(it == it_end)
        return false;
    else
        m_retry_agent_list.erase(it);

    return true ;
}

//不需要重试，将对应的agent从链表中移除，并且执行析构操作
void ExtentTask::agent_exec_extent_back(uint32_t agent_index , bool recycle)
{
    //无论是重试返回还是第一次返回都会查找这个list，如果查找成功，说明是第一次返回
    //如果在该函数中查找不到，那么就说明重试的操作成功了，从retry_list中移除
    //并且不再需要判断extent_list的长度进行回调了
    bool remove_ret = remove_agent_from_extent_list(agent_index);
    if(!remove_ret)
    {
        //如果查找不成功说明是重试操作，再将它从重试链表上移除
        if(!remove_agent_from_retry_list(agent_index))
        {
            LOG_FATAL("ExtentTask::Unknown error can not find index in retry list index " 
                     + int_to_str(agent_index));
            return ;
        }
    }

    //如果是对端关闭了，基类已经delete这个Agent了，不再删除
    if(recycle)
    {
        AgentRegister::getInstance()->delete_SC_agent(agent_index);
    }

    //只有在第一次成功的时候才会执行判断和回调操作
    if(remove_ret)
    {
        //对于Agent的第一次回调，可能有两种路径：1、成功调用该函数；2、失败调用retry
        //如果都调用了当前函数，说明都成功，并且都会将Agent从extent list中移除
        //因此如果是第一次返回并且该链表已空，说明扩展操作都成功了，执行会回调
        //流程是这样规定的，当第一轮全部返回的时候就会执行回调（告诉MPC）
        //如果有尚未完成的agent，在CS内部继续重试!
        if(m_extent_agent_list.empty())
        {
            m_state = RETRY;
            LOG_EVENT("All extent agent back ! do CALLBACK");
            //对于MPC的请求，除非更新数据库失败，否则只能回复成功，重试其余的失败节点
            do_last_work(true , std::string());
            return ;
        }
    }
    else
    {
        //对于重试的节点的每一次成功都会判断是否是最后一个，如果是将自身删除
        //必须保证接下来不再执行任何操作，没有任何引用！
        if(m_retry_agent_list.empty())
        {
            LOG_EVENT("all retry agent success ! DELETE MYSELF");
            TaskManager::getInstance()->delete_extent_task(m_type);
            return ;
        }
    }
}

//出现错误的节点，重新尝试，不过需要将它们加入到另外的节点上去了
//重新尝试不再使用原来的Agent，析构原来的Agent，重新创建一个Agent
//每次都是短连接
void ExtentTask::retry_do_extent(uint32_t agent_index)
{
    uint32_t new_agent_index = 0;
    bool remove_ret = false;

    remove_ret = remove_agent_from_extent_list(agent_index);
    //无论如何都会将这个index从retry队列上移除，重新创建一个Agent
    remove_agent_from_retry_list(agent_index);

    AgentRegister::getInstance()->delete_SC_agent(agent_index);

    //首先查看对应的IP了，到map上查找
    std::map<uint32_t,uint32_t>::iterator it_find = m_index_ip_map.find(agent_index);  
    if(it_find == m_index_ip_map.end())
    {
        LOG_FATAL("ExtentTask::Unknown error:can not find index to ip from map ! index : " 
                + int_to_str(agent_index));

        goto GIVE_UP;
    }
    else 
    {
        std::cerr<<"!!!!!!!!!!!!!!!!retry SU extent !!!!!!!!!!!!!"<<std::endl;
        //将原来的清除掉
        m_index_ip_map.erase(it_find);
        //重新创建一个短连接，对节点进行连接尝试,回调函数依旧
        if(create_agent_and_init(it_find->second , new_agent_index) < 0)
        {
            LOG_ERROR("ExtentTask::create and init agent error when retry !");
            goto GIVE_UP ;
        }
        else 
        {
            //如果成功，将新的index添加到retry链表上
            m_retry_agent_list.push_back(new_agent_index);
        }
    }

    return ;

GIVE_UP :
    give_up_this_node(agent_index);
}

void ExtentTask::give_up_this_node(uint32_t agent_index)
{
    //将这个index从retry链表上移除,不再重试
    remove_agent_from_retry_list(agent_index);
}

void ExtentTask::do_callback(bool ret)
{
    //执行回调的时候
    if(0 == m_father_agent_index)
    {
        m_callback_func(ret , m_type , m_ext_info);
        return ;
    }
    if(AgentRegister::getInstance()->get_agent_from_index(m_father_agent_index) == NULL)
    {
        LOG_ERROR("ExtentTask::Opposite MPC Agent has been closed ! delete this ...");
        TaskManager::getInstance()->delete_extent_task(m_type);
    }
    else 
        m_callback_func(ret , m_type , m_ext_info);
}

void ExtentTask::do_last_work(bool ret , const std::string &res)
{
    m_extent_ret = ret;
    m_item->set_operation_result(ret , res);
    m_item->set_callback(std::tr1::bind(&ExtentTask::do_last_back , 
                this , 
                std::tr1::placeholders::_1));

    Dispatcher::getInstance()->send_request(m_item);
}

void ExtentTask::do_last_back(bool ret)
{
    if(!ret)
    {
        //假设这一步总是返回成功的！
    }

    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }

    this->do_callback(m_extent_ret);
}
