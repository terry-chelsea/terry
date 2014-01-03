/*
 * =====================================================================================
 *
 *       Filename:  Migration_task.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 17:49:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Migration_task.h"
#include "AgentRegister.h"
#include "CSConfiguration.h"
#include "CS_Epoll.h"
#include "Log_warp.h"
#include "Lib_tool.h"
#include "../common/protocol/protocol.h"

#include "Migration_work_item.h"
#include "Dispatcher.h"


//初始化工作，保存信息
MigrationTask::MigrationTask(uint32_t agent_index , RULE_TYPE type , struct Migration_Info info)
    :Task(agent_index) , 
    m_type(type) , 
    m_agent_index(0) ,
    m_mig_info(info) , 
    m_item(NULL) ,
    m_retry_time(0) , 
    m_dest_migrate_ret(false)
{

}

//析构的时候需要保证创建的Agent已经被回收了
MigrationTask::~MigrationTask()
{
    if(m_agent_index != 0)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
        m_agent_index = 0;
    }

    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }
}

int MigrationTask::init(tr1::function<void (bool , RULE_TYPE , struct Migration_Info)> ret_callback)
{
    //保存回调函数
    m_callback_func = ret_callback;

    std::string ip_str;


    WorkItem *item = new MigrationWorkItem(m_type , m_mig_info);

    item->init(std::tr1::bind(&MigrationTask::after_log_operation , this , 
                std::tr1::placeholders::_1));

    Dispatcher::getInstance()->send_request(item) ;

    //保存，等到收到结果之后删除
    m_item = item;

    return 0;

}

void MigrationTask::after_log_operation(bool ret)
{
    if(!ret)
    {
        //暂时这个回复只是成功!
    }

    std::string load_string;
    std::string out_data;
    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    SCAgent *agent = NULL ;

    cstore::pb_MSG_CS_MU_MIGRATE_BUCKET   pb_mu_info;
    cstore::pb_MSG_CS_SU_MIGRATE_BUCKET   pb_su_info;
    
    if(MU_RULER == m_type)
    {
        header.cmd = MSG_CS_MU_MIGRATE_BUCKET;
        //只需要源地址和待迁移的桶号
        pb_mu_info.set_source_mu(m_mig_info.src_ip);
        pb_mu_info.set_bucket_index(m_mig_info.bucket_nr);
        if((!pb_mu_info.IsInitialized()) || (!pb_mu_info.SerializeToString(&load_string)))
        {
            LOG_ERROR("MigrationTask::protobuf MU migration info Initialized error !");
            goto CREATE_ERR;
        }
    }
    else
    {
        header.cmd = MSG_CS_SU_MIGRATE_BUCKET;
        pb_su_info.set_ip(m_mig_info.src_ip);
        pb_su_info.set_bucket_index(m_mig_info.bucket_nr);
        if((!pb_su_info.IsInitialized()) || (!pb_su_info.SerializeToString(&load_string)))
        {
            LOG_ERROR("MigrationTask::protobuf SU migration info Initialized error !");
            goto CREATE_ERR;
        }
    }
    //创建连接目的SU或者MU的agent,首先通知目的SU执行迁移操作
    if(MU_RULER == m_type)
        agent = Task::create_new_agent(m_mig_info.dest_ip , 
                    CSConfiguration::getInstance()->mu_port);
    else 
        agent = Task::create_new_agent(m_mig_info.dest_ip , 
                    CSConfiguration::getInstance()->su_port);

    if(NULL == agent)
    {
        LOG_ERROR("MigrationTask::create new agent error !");
        goto CREATE_ERR;
    }
    //保存index，等到收到回复的时候析构Agent
    m_agent_index = agent->get_agent_index();
    std::cerr<<"before migration dest agent index "<<m_agent_index<<std::endl;
     
    header.length = load_string.size();
    //将数据打包封装在out_data中在初始化agent，执行connect操作
    agent->do_package(header , load_string , out_data);

    //这里不需要用户传递自己的数据,设置为NULL 
    //在Agent执行init函数的时候需要指定这个Agent发送的数据，一次交互，短连接
    if(agent->init(std::tr1::bind(&MigrationTask::done_dest_migration , 
                    this ,
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3) , out_data , NULL) < 0)
    {
        LOG_ERROR("MigrationTask::agent init error !");
        goto ERR ;
    }

    std::cerr<<"------------------ Migration task infomation ------------------"<<std::endl;
    std::cerr<<"Create a Migration Task successfully! send data to destention Node , IP : "<<string_ip(m_mig_info.dest_ip)<<std::endl;

    return ;
    //如果这里出现错误，不需要执行回调，只需要删除已经创建的agent就可以了
ERR :
    AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    m_agent_index = 0;
CREATE_ERR : 
    migration_exec_back(false);
    return ;
}

void MigrationTask::done_dest_migration(void *req , RET_TYPE type , void *data)
{
    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    InReq *back_req = NULL;
    
    //无论如何在收到回复之后都要回收Agent,index设置为0表示Agent不存在
    std::cerr<<"after migration dest agent index "<<m_agent_index<<std::endl;

    if(UNKOWN_ERROR != type)
    {
        //如果是这个错误，说明Agent已经被回收了，不需要再次回收
        AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    }
    else
    {
        LOG_ERROR("MigrationTask::the opposite close this agent of migrate op!");
    }

    m_agent_index = 0;
    //首先判断SCAgent的操作是否成功，如果不成功直接回调
    if(Task::parse_callback_ret(req , type) < 0)
    {
        goto MIG_ERR;
    }

    back_req = (InReq *)req;
    header = back_req->m_msgHeader;

    std::cerr<<"------------------ Migration task infomation ------------------"<<std::endl;
    std::cerr<<"destion node reply OK header error is : "<<header.error<<std::endl;
    
    //如果是MU的操作，那么收到回复之后判断回复报文是否正确
    //如果正确就发起数据库更新的操作
    if(MU_RULER == m_type)
    {
        if(header.cmd != MSG_CS_MU_MIGRATE_BUCKET_ACK)
        {
            LOG_ERROR("MigrationTask::Undefined MU ack cmd type : " + int_to_str(header.cmd));
            goto MIG_ERR;
        }
        //报文头正确，在判断迁移操作的结果
        else 
        {
            migration_exec_back(MU_OK == header.error);
        }
    }

    //更新数据库操作
    //剩余的操作都是SU执行结果的操作了
    else
    {
        if(header.cmd != MSG_CS_SU_MIGRATE_BUCKET_ACK)
        {
            LOG_ERROR("MigrationTask::Undefined SU ack cmd type : " + int_to_str(header.cmd));
            goto MIG_ERR;
        }
        else
        {
            migration_exec_back(SU_OK == header.error);
        }
    }

    return ;

    //迁移操作失败，直接回复
MIG_ERR :
    migration_exec_back(false);
}

//其实这个函数的返回值应该是void，迁移的多步操作是一个线性的流程，任何一步出错都可以直接通知回调，而不需要向调用者汇报处理结果。
int MigrationTask::migration_exec_back(bool ret)
{
    //记录执行结果，接下来记录操作的结果!
    m_dest_migrate_ret = ret;    
    //无论是MU还是SU都需要执行更新数据库规则的操作 , 在更新完数据库之后还需要通知RS
    //如果通知RS完成之后才算是更新数据库成功，如何保证这两个操作的一致性呢？
    update_rule_and_db();
    
    return 0;
}

int MigrationTask::update_rule_and_db()
{
    std::string some_info;
    m_item->set_operation_result(m_dest_migrate_ret , some_info);
    m_item->set_callback(std::tr1::bind(&MigrationTask::after_update_db , 
                this , 
                std::tr1::placeholders::_1));

    Dispatcher::getInstance()->send_request(m_item);

    return 0;
}

void MigrationTask::after_update_db(bool ret)
{
    std::string load_string;
    std::string out_data;

    SCAgent *agent = NULL;
    //到此时workitem操作完成，可以释放
    if(m_item != NULL)
    {
        delete m_item;
        m_item = NULL;
    }

    //如果第一步操作就出错，不需要关心数据库是否修改成功
    if(!m_dest_migrate_ret)
    {
        this->do_callback(false);
        return ;
    }

    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    cstore::pb_MSG_CS_SU_DELETE_BUCKET pb_del_info;

    //如果数据库修改成功，修改内存中的规则
    if(ret)
    {
        RulerManager::getInstance()->new_migration_order(m_mig_info.bucket_nr , m_mig_info.src_ip , m_mig_info.dest_ip , m_type);
    }

    //如果数据库修改失败，继续完成迁移操作,而不修改内存
    //如果是MU的桶迁移，返回等待更新数据库操作的完成
    //对于MU，迁移工作就到修改数据库规则就结束了，执行回调。
    if(MU_RULER == m_type)
    {
        do_callback(ret);
        
        return ;
    }

    uint32_t dest_ip = 0;
    unsigned long dest = 0;
    //设置删除源桶的操作没有完成，如果出错将它设置为true，但是设置错误
    //如果是SU，还需要再创建Agent向源IP发送报文
    //如果是执行失败，说明修改文件失败，那么就将目的IP上的桶执行删除操作
    //如果执行成功，说明修改文件成功，那么僵源IP上的桶执行删除操作
    if(ret)
    {
        dest_ip = m_mig_info.src_ip;
        dest = 1;
    }
    else
    {
        dest_ip = m_mig_info.dest_ip;
        dest = 0;
    }

    header.cmd = MSG_CS_SU_DELETE_BUCKET;
    pb_del_info.set_bucket_index(m_mig_info.bucket_nr);

    if((!pb_del_info.IsInitialized()) || (!pb_del_info.SerializeToString(&load_string)))
    {
        LOG_ERROR("MigrationTask::protobuf delete bucket string initialized error !");
        goto ERR ;
    }

    std::cerr<<"-----------SU migration after modify rule : --------------"<<std::endl;
    std::cerr<<"next send info to delete IP : "<<string_ip(dest_ip)<<std::endl;
    std::cerr<<pb_del_info.DebugString()<<std::endl;

    agent = Task::create_new_agent(dest_ip , 
            CSConfiguration::getInstance()->su_port);

    if(NULL == agent)
    {
        LOG_ERROR("MigrationTask::creatr new agent error !");
        goto ERR ;
    }
    m_agent_index = agent->get_agent_index();
    std::cerr<<"before delete agent index is : "<<m_agent_index<<std::endl;

    //打包数据，然后设置回调函数，不携带私有数据
    agent->do_package(header , load_string , out_data);
    if(agent->init(std::tr1::bind(&MigrationTask::done_del_bucket , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3) , out_data , (void *)dest) < 0)
    {
        LOG_ERROR("MigrationTask::agent to src delete init error !");
        goto DEL_AGENT;
    }

    return ;

DEL_AGENT:
    AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    m_agent_index = 0;

    //出错了，但是标志删除操作已经完成，但是操作执行错误,这时候更新数据库操作
    //不可能完成，因此不需要检查回调
ERR :
    do_callback(false);
}

//收到删除源桶的操作的回复，判断是否成功
void MigrationTask::done_del_bucket(void *req , RET_TYPE type , void *data)
{
    unsigned long dest = (unsigned long)data;
    bool del_back_ret = false;
    //无论如隔在收到回复之后都要回收Agent,index设置为0表示Agent不存在
    std::cerr<<"after delete buckte agent is "<<m_agent_index<<std::endl;
    if(UNKOWN_ERROR != type)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    }
    else
    {
        LOG_ERROR("MigrationTask::the opposite close this agent of migrate op!");
    }
    m_agent_index = 0;

    //如果执行失败标志错误
    if(parse_callback_ret(req , type) < 0)
        del_back_ret = false;
    else
    {
        InReq *back_req = (InReq *)req;
        MsgHeader header = back_req->m_msgHeader;

        std::cerr<<"--------------migration infomation-------------"<<std::endl;
        std::cerr<<"source mode reply delete bucket , header error is : "<<header.error<<std::endl;

        if(MSG_CS_SU_DELETE_BUCKET_ACK != header.cmd)
        {
            LOG_ERROR("MigrationTask::Undefined del ack cmd type : " + int_to_str(header.cmd));
            del_back_ret = false;
        }
        else if(SU_OK != header.error)
        {
            LOG_ERROR("MigrationTask::SU delete source bucket error !");
            del_back_ret = false;
        }
        else
        {
            LOG_EVENT("SU execurate delete source bucket successfully !");
            del_back_ret = true;
        }
    }

    //如果是修改规则出现的错误，不再关心删除桶（此时删除的是目的IP上的桶）是否成功
    //直接返回错误，如果修改正确并且删除正确，通知目的节点进入恢复状态。
    if(0 == dest)
        do_callback(false);
    else if(del_back_ret)
        notice_dest_recovery();
    else
        do_callback(false);
}

//执行到这一步说明前三步都已经正确完成了，那么剩下最后一步，既然规则也该了。源桶也删了，
//一步出错就会导致我们损失一个桶（因为桶只有处于运行状态才可能对外服务），所以需要出错重试
void MigrationTask::notice_dest_recovery()
{
    std::string out_data;
    std::string pb_data;
    SCAgent *agent = NULL;
    cstore::pb_MSG_CS_SU_SET_BUCKET_STATE set_state_pb;
    MsgHeader header;
    memset(&header , 0 , sizeof(header));

    set_state_pb.set_bucket_index(m_mig_info.bucket_nr);
    set_state_pb.set_state(cstore::RECOVERY);

    if(!set_state_pb.IsInitialized() || !set_state_pb.SerializeToString(&pb_data))
    {
        LOG_ERROR("MigrationTask::protobuf error when doing dest node recovery");
        goto ERR;
    }
    std::cerr<<"----------------Migration Info-----------------"<<std::endl;
    std::cerr<<"recovery infomation send data to IP : "<<string_ip(m_mig_info.dest_ip)<<std::endl;
    std::cerr<<set_state_pb.DebugString()<<std::endl;

    agent = Task::create_new_agent(m_mig_info.dest_ip , 
            CSConfiguration::getInstance()->su_port);
    if(NULL == agent)
    {
        LOG_ERROR("MigrationTask::create new Agent for recovery failed ");
        goto ERR;
    }
    m_agent_index = agent->get_agent_index();

    header.cmd = MSG_CS_SU_SET_BUCKET_STATE;
    header.length = pb_data.size();
    agent->do_package(header , pb_data , out_data);
    if(agent->init(std::tr1::bind(&MigrationTask::after_set_recovery , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3) , out_data , NULL) < 0)
    {
        LOG_ERROR("MigrationTask::Agent for set recovery error !");
        goto DEL_ERR;
    }

    return ;

DEL_ERR : 
    AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    m_agent_index = 0;

ERR :
    retry_set_retry();
}

void MigrationTask::after_set_recovery(void *req , RET_TYPE type , void *data)
{
    bool set_back_ret = false;
    //无论如隔在收到回复之后都要回收Agent,index设置为0表示Agent不存在
    if(UNKOWN_ERROR != type)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_agent_index);
    }
    else
    {
        LOG_ERROR("MigrationTask::the opposite close this agent of migrate op!");
    }
    m_agent_index = 0;

    std::cerr<<"----------------Migration Info-----------------"<<std::endl;
    std::cerr<<"get set recovery back !"<<std::endl;

    //如果执行失败标志错误
    if(parse_callback_ret(req , type) < 0)
        set_back_ret = false;
    else
    {
        InReq *back_req = (InReq *)req;
        MsgHeader header = back_req->m_msgHeader;
        if(MSG_CS_SU_SET_BUCKET_STATE_ACK != header.cmd)
        {
            LOG_ERROR("MigrationTask::Undefined set recovery ack amd type : " + 
                    int_to_str(header.cmd));
            set_back_ret = false;
        }
        else if(SU_OK != header.error)
        {
            LOG_ERROR("MigrationTask::SU set recovery state error !");
            set_back_ret = false;
        }
        else 
        {
            LOG_EVENT("SU execurate set bucket state recovery successfully !");
            set_back_ret = true;
        }
    }
    
    if(set_back_ret)
    {
        std::cerr<<"-------------------Operation do callback successfully !-------------"<<std::endl;
        do_callback(true);
    }
    else 
        retry_set_retry();
}

void MigrationTask::retry_set_retry()
{
    std::cerr<<"-------------Migration Info --------------"<<std::endl;
    std::cerr<<"doing retry time is "<<m_retry_time<<std::endl;
    ++ m_retry_time;
    if(m_retry_time < CSConfiguration::getInstance()->migration_retry_time)
    {
        notice_dest_recovery();
    }
    else 
    {
        do_callback(false);
    }
}

//#include "MPCClientAgent.h"
//执行回调函数之前应该首先查看对于的Agent是否仍然存在，如果不存在，直接delete自己
//从下面可以看出回调和集成之间的差别，如果是回调，不需要知道回调的对象是对，只需要调用这个
//回调函数就可以了，而集成要求基类必须具有这个接口，或者使用dynamic_cast进行墙砖！
void MigrationTask::do_callback(bool ret)
{
    //执行回调的时候
    if(0 == m_father_agent_index)
    {
        m_callback_func(ret , m_type , m_mig_info);
        return ;
    }
    //如果上层的Agent已经删除了，因为Agent删除不会通知Task
    if(AgentRegister::getInstance()->get_agent_from_index(m_father_agent_index) == NULL)
    {
        LOG_ERROR("MigrationTask::Opposite MPC Agent has been closed ! delete this ...");
        delete this;
    }
    else 
    {
        m_callback_func(ret , m_type , m_mig_info);
    }
}
