/*
 * =====================================================================================
 *
 *       Filename:  MPCClientAgent.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 20:40:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Log_warp.h"
#include "Lib_tool.h"
#include "MPCClientAgent.h"
#include "Task_manager.h"
#include "Extent_task.h"
#include "Rule_file_interface.h"
#include "HB_manager.h"

//被ListenAgent创建出来的对象...
MPCClientAgent::MPCClientAgent(const TCPSocket &sock , const SocketAddress &addr , Epoll *epl)
    :CSTCPAgent(sock , addr , epl) ,
    m_migrate_task(NULL) , 
    m_init_task(NULL)
{

}

MPCClientAgent::~MPCClientAgent()
{
    /*  
    if(m_migrate_task != NULL)
    {
        delete m_migrate_task;
        m_migrate_task = NULL;
    }
    */
    //删除的时候不通知下面的Task，而是等到Task完成之后删除它。
}

void MPCClientAgent::readBack(InReq &req)
{
    uint32_t cmd = req.m_msgHeader.cmd;

    int ret = -1;
    switch(cmd)
    {
        //需要CS控制，并且需要修改数据库
        case MSG_MPC_CS_MU_MIGRATE_BUCKET :
        {
            ret = do_migrate_bucket(MU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do mu migrate bucket error !");
            }
            break ;
        }
        case MSG_MPC_CS_SU_MIGRATE_BUCKET :
        {
            ret = do_migrate_bucket(SU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do su migrate bucket error !");
            }
            break ;
        }
        case MSG_MPC_CS_MU_EXTENT_BUCKET :
        {
            ret = do_extent_bucket(MU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do mu extent bucket error !");
            }
            break ;
        }
        case MSG_MPC_CS_SU_EXTENT_BUCKET :
        {
            ret = do_extent_bucket(SU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do su extent bucket error !");
            }
            break ;
        }
        //从内存中读取全部的规则表信息
        case MSG_MPC_CS_GET_MU_RULE :
        {
            ret = do_get_hash_rule(MU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do get all mu rule error !");
            }
            break ;
        }
        case MSG_MPC_CS_GET_SU_RULE :
        {
            ret = do_get_hash_rule(SU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do get all su rule error !");
            }
            break ;
        }
        //根据各个节点心跳信息，发送每个节点的桶和负载
        case MSG_MPC_CS_MU_BUCKET :
        {
            ret = do_get_last_bucket_info(MU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do get last mu bucket info error !");
            }
            break ;
        }
        case MSG_MPC_CS_SU_BUCKET :
        {
            ret = do_get_last_bucket_info(SU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do get last su bucket info error !");
            }
            break ;
        }
        //垃圾回收单独实现
        case MSG_MPC_CS_RUBBISH_RECYCLE :
        {
            ret = do_rubbish_recycle(req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do rubbish recycle error !");
            }
            break ;
        }
        // 获取垃圾回收的桶的信息
        case MSG_MPC_CS_GLOBAL_BLOCK_INFO :
        {
            ret = do_global_bucket_info(req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do get global bucket info error !");
            }
            break;
        }
        //初始化数据库的操作，需要根据此信息修改数据库
        case MSG_MPC_CS_MU_INIT_HASH :
        {
            ret = do_init_hash_rule(MU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do init mu hash rule error !");
            }
            break ;
        }
        case MSG_MPC_CS_SU_INIT_HASH :
        {
            ret = do_init_hash_rule(SU_RULER , req);
            if(ret < 0)
            {
                LOG_ERROR("MPCClientAgent::do init su hash rule error !");
            }
            break ;
        }

        default :
            LOG_ERROR("MPCClientAgent::Undefined cmd type : " + int_to_str(cmd));
            std::string empty_data;
            CSTCPAgent::reply_request(0 , CS_OK , 0 , 0 , empty_data);
            break ;

    }
}

#include "rubbish_recycler.h"
int MPCClientAgent::do_rubbish_recycle(InReq &req)
{
    std::string empty_data;
    LOG_EVENT("Get rubbish recycle command , return ...");
//    cstore::pb_MSG_MPC_CS_RUBBISH_RECYCLE info;
//    std::string in_data(req.ioBuf , req.m_msgHeader.length);
//    if(!info.ParseFromString(in_data) || !info.IsInitialized())
//    {
//        LOG_ERROR("Paser protobuf error in rubbish recycler !");
//        goto ERR;
//    }
    if(RCManager::getInstance()->start_do_recylcer(CSTCPAgent::m_index , 100000000 , 0.01) < 0)
    {
        LOG_ERROR("work for rubbish recycler failed !");
        goto ERR;
    }

    CSTCPAgent::reply_request(MSG_MPC_CS_RUBBISH_RECYCLE_ACK , CS_OK 
            , 0 , 0 , empty_data);

    return 0;

ERR :
    CSTCPAgent::reply_request(MSG_MPC_CS_RUBBISH_RECYCLE_ACK , 
            CS_ERROR , 0 , 0 , empty_data);
    return -1;
}

int MPCClientAgent::do_global_bucket_info(InReq &req)
{
    HBManager *manager = HBManager::getInstance();
    std::string out_data;
    std::map<uint32_t , struct Node_Info>::iterator su_it = manager->map_begin(SU_RULER);
    std::map<uint32_t , struct Node_Info>::iterator su_end = manager->map_end(SU_RULER);
    std::map<uint32_t , struct Node_Info>::iterator mu_it = manager->map_begin(MU_RULER);
    std::map<uint32_t , struct Node_Info>::iterator mu_end = manager->map_end(MU_RULER);
    
    cstore::pb_MSG_MPC_CS_GET_BLOCK_NUM_ACK info_ack;
    cstore::Node_Block *block_ptr = NULL;
    for( ; su_it != su_end ; ++ su_it)
    {
        block_ptr = info_ack.add_su_node_list();
        struct Node_Info node_info = su_it->second;
        int bucket_num = node_info.bucket_list.size();
        int sum_block_num = 0;
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            sum_block_num += node_info.bucket_list[i].bucket_load;
        }
        block_ptr->set_storage_limit(node_info.storage_limit / (1024 * 1024));
        block_ptr->set_bucket_number(bucket_num);
        block_ptr->set_block_number(sum_block_num);
        block_ptr->set_node_ip(su_it->first);
    }
    for( ; mu_it != mu_end ; ++ mu_it)
    {
        block_ptr = info_ack.add_mu_node_list();
        struct Node_Info node_info = mu_it->second;
        int bucket_num = node_info.bucket_list.size();
        int sum_block_num = 0;
        for(int i = 0 ; i < bucket_num ; ++ i)
        {
            sum_block_num += node_info.bucket_list[i].bucket_load;
        }
        block_ptr->set_storage_limit(node_info.storage_limit / (1024 * 1024));
        block_ptr->set_bucket_number(bucket_num);
        block_ptr->set_block_number(sum_block_num);
        block_ptr->set_node_ip(mu_it->first);
    }
    info_ack.set_su_dup(RulerManager::getInstance()->get_dup_num(SU_RULER));
    info_ack.set_mu_dup(RulerManager::getInstance()->get_dup_num(MU_RULER));

    if(!info_ack.IsInitialized() || !info_ack.SerializeToString(&out_data))
    {
        LOG_ERROR("MPCClientAgent::protobuf Initialized error for bucket info!");
        goto ERR;
    }
    std::cerr<<"-----------Bucket Infomation For Recycle-------------"<<std::endl;
    std::cerr<<info_ack.DebugString()<<std::endl;

    CSTCPAgent::reply_request(MSG_MPC_CS_GLOBAL_BLOCK_INFO_ACK , 
            CS_OK , 0 , 0 , out_data);
    return 0;

ERR:
    out_data.clear();
    CSTCPAgent::reply_request(MSG_MPC_CS_GLOBAL_BLOCK_INFO_ACK , 
            CS_ERROR , 0 , 0 , out_data);
    
    return -1;
}

//没有负载数据
int MPCClientAgent::do_get_hash_rule(RULE_TYPE type , InReq &req)
{
    RulerManager *rule_manager_ptr = RulerManager::getInstance();
    uint32_t current_bucket_num = rule_manager_ptr->get_bucket_number(type);
    bool ret = false;
    std::string out_data;
    uint32_t  cmd_ack = 0;
    HashRule rule_item;

    std::cerr<<"here"<<std::endl;
    std::cerr<<"##########################"<<std::endl;;
    std::cerr<<"Begin get all "<<(type == MU_RULER ? "MU" : "SU")<<" Rule !"<<std::endl<<std::endl;

    cstore::pb_MSG_MPC_CS_GET_MU_RULE_ACK  mu_rule_ack;
    cstore::pb_MSG_MPC_CS_GET_SU_RULE_ACK  su_rule_ack;
    cstore::Map_Item                      *map_item_ptr = NULL;

    for(uint32_t i = 0 ; i < current_bucket_num ; ++ i)
    {
        memset(&rule_item , 0 , sizeof(rule_item));
        if(rule_manager_ptr->get_hash_rule(i , type , rule_item) < 0)
        {
            LOG_ERROR("MPCClientAgent::get hash rule of bucket : " + int_to_str(i) + " error !");
            goto ERR ;
        }
        if(MU_RULER == type)
            map_item_ptr = mu_rule_ack.add_map_bucket();
        else
            map_item_ptr = su_rule_ack.add_map_bucket();

        hash_item_to_map_item(map_item_ptr , rule_item);
    }

    if(MU_RULER == type)
    {
        ret = (!mu_rule_ack.IsInitialized()) || (!mu_rule_ack.SerializeToString(&out_data));
        cmd_ack = MSG_MPC_CS_GET_MU_RULE_ACK;
        std::cerr<<"------------Get All Rule------------"<<std::endl;
        std::cerr<<mu_rule_ack.DebugString()<<std::endl;
    }
    else
    {
        ret = (!su_rule_ack.IsInitialized()) || (!su_rule_ack.SerializeToString(&out_data));
        cmd_ack = MSG_MPC_CS_GET_SU_RULE_ACK;

        std::cerr<<"------------Get All Rule------------"<<std::endl;
        std::cerr<<su_rule_ack.DebugString()<<std::endl;
    }

    if(ret)
    {
        LOG_ERROR("MPCClientAgent::protobuf initialized error !");
        goto ERR;
    }
    
    
    CSTCPAgent::reply_request(cmd_ack , CS_OK , 
            0 , 0 , out_data);
    
    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 
            0 , 0 , out_data);

    return -1;
}

void MPCClientAgent::hash_item_to_map_item(cstore::Map_Item *map_item , const HashRule &rule_item)
{
    uint32_t ip = INVALID_IP;

    for(int i = 0 ; i < MAX_IP_NUM ; ++ i)
    {
        ip = rule_item.ip[i];
        if(INVALID_IP == ip)
            break ;

        map_item->add_module_ip(ip);
    }

    map_item->set_bucket_num(rule_item.bucket_nr);
}

int MPCClientAgent::do_get_last_bucket_info(RULE_TYPE type, InReq &req)
{
    std::map<uint32_t , struct Node_Info >::iterator  it;
    std::map<uint32_t , struct Node_Info >::iterator  it_end;
    HBManager *hb_manager = HBManager::getInstance();

    bool ret = false;
    std::string out_data;
    uint32_t  cmd_ack = 0;

    std::cerr<<"*******************************"<<std::endl;
    std::cerr<<"Get Last Bucket Infomations!"<<std::endl<<std::endl;
    
    cstore::pb_MSG_MPC_CS_MU_BUCKET_ACK    mu_bucket_ack;
    cstore::pb_MSG_MPC_CS_SU_BUCKET_ACK    su_bucket_ack;
    cstore::Bucket_List                   *bucket_list_ptr = NULL;

    it = hb_manager->map_begin(type);
    it_end = hb_manager->map_end(type);

    if(MU_RULER == type)
    {
        cmd_ack = MSG_MPC_CS_MU_BUCKET_ACK;
        if(it == it_end)
            goto RET ;

        for( ; it != it_end ; ++ it)
        {
            bucket_list_ptr = mu_bucket_ack.add_bucket_list();
            bucket_list_ptr->set_mu_ip(it->first);
            bucket_info_to_bucket_list(bucket_list_ptr , it->second.bucket_list);
        }
    }
    else
    {
        cmd_ack = MSG_MPC_CS_SU_BUCKET_ACK;
        if(it == it_end)
            goto RET ;

        for( ; it != it_end ; ++ it)
        {
            bucket_list_ptr = su_bucket_ack.add_bucket_list();
            bucket_list_ptr->set_mu_ip(it->first);
            bucket_info_to_bucket_list(bucket_list_ptr , it->second.bucket_list);
        }
    }
    
    if(MU_RULER == type)
    {
        ret = ((!mu_bucket_ack.IsInitialized()) || !mu_bucket_ack.SerializeToString(&out_data));

        std::cerr<<"---------------Last SU Bucket Sataus---------------"<<std::endl;
        std::cerr<<mu_bucket_ack.DebugString()<<std::endl;
    }
    else
    {
        ret = ((!su_bucket_ack.IsInitialized()) || !su_bucket_ack.SerializeToString(&out_data));
        std::cerr<<"---------------Last SU Bucket Sataus---------------"<<std::endl;
        std::cerr<<su_bucket_ack.DebugString()<<std::endl;
    }

    if(ret)
    {
        LOG_ERROR("MPCClientAgent::protobuf initialized error !");
        goto ERR;
    }
RET :

    CSTCPAgent::reply_request(cmd_ack , CS_OK , 0 , 0 , out_data);
    
    return 0;

ERR :
    out_data.clear();
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 0 , 0 , out_data);

    return -1;
}

void MPCClientAgent::bucket_info_to_bucket_list(cstore::Bucket_List *bucket_list , const vector<Bucket_Info> &info)
{
    cstore::Bucket_Item   *bucket_item_ptr = NULL;
    int node_bucket_num = info.size();
    struct Bucket_Info  bucket_info;

    for(int i = 0 ; i < node_bucket_num ; ++ i)
    {
        bucket_info = info[i];
        bucket_item_ptr = bucket_list->add_bucket_item();
        bucket_item_ptr->set_bucket(bucket_info.bucket_nr);
        bucket_item_ptr->set_bucket_load(bucket_info.bucket_load);
    }
}

//如果只有副本数和模数而没有地址，说明是测试系统是否初始化，如果已经初始化返回一个error
int MPCClientAgent::do_init_hash_rule(RULE_TYPE type , InReq &req)
{
    bool ret = false;
    int ret_value = -1;
    std::string out_data;
    uint32_t cmd_ack = 0;
    MsgHeader header = req.m_msgHeader;
    std::string input_data(req.ioBuf , header.length);
    cstore::pb_MSG_MPC_CS_SU_INIT_HASH    in_init_su;
    cstore::pb_MSG_MPC_CS_MU_INIT_HASH    in_init_mu;

    int ip_scope_size = 0;
    int globle_mod = 0;
    int g_duplicate_num = 0;

    vector<cstore::IP_scope> ip_scope_vec;
    Init_info info;
    InitSystemTask *task = NULL;
    
    if(MU_RULER == type)
    {
        cmd_ack = MSG_MPC_CS_MU_INIT_HASH_ACK;
        ret = in_init_mu.ParseFromString(input_data);
    }
    else
    {
        cmd_ack = MSG_MPC_CS_SU_INIT_HASH_ACK;
        ret = in_init_su.ParseFromString(input_data);
    }
    /*  
    if(!RulerManager::getInstance()->can_init(type))
    {
        CSTCPAgent::reply_request(cmd_ack , CS_OK , 
            RULE_HAS_INITED , 0 , out_data);
        return 0; 
    }
    */

    //解析错误
    if(!ret)
    {
        LOG_ERROR("MPCClientAgent::Parse input data to protobuf error !");
        goto ERR;
    }
    
    if(MU_RULER == type)
    {
        std::cerr<<in_init_mu.DebugString()<<std::endl;
        ip_scope_size = in_init_mu.module_ip_size();
        globle_mod = in_init_mu.mod();
        g_duplicate_num = in_init_mu.duplicate_num();
        //!!!!!!!!!!!!!!!!!!!!!!!
//        g_duplicate_num = 2;
        //!!!!!!!!!!!!!!!!!!!!!!!
        for(int i = 0 ; i < ip_scope_size ; ++ i)
        {
            ip_scope_vec.push_back(in_init_mu.module_ip(i));
        }
    }
    else
    {
        std::cerr<<in_init_su.DebugString()<<std::endl;
        ip_scope_size = in_init_su.module_ip_size();
        globle_mod = in_init_su.mod();
        g_duplicate_num = in_init_su.duplicate_num();
        for(int i = 0 ; i < ip_scope_size ; ++ i)
        {
            ip_scope_vec.push_back(in_init_su.module_ip(i));
        }
    }
      
    if(0 == ip_scope_size)
    {
        std::cerr<<"just test system has inited infomation !"<<std::endl;
        CSTCPAgent::reply_request(cmd_ack , CS_OK , 
            RULE_NOT_INITED , 0 , out_data);
        return 0; 
    }
    
    ret_value = init_globle_hash_rule(type , ip_scope_vec , globle_mod , 
            g_duplicate_num , info.all_ip_list);
    info.dup = g_duplicate_num;
    info.mod = globle_mod;

    if(-1 == ret_value)
        goto ERR ;
    else if(-2 == ret_value)
        goto INPUT_ERR ;

    task = new InitSystemTask(CSTCPAgent::m_index , type , info);
    if(NULL == task)
    {
        LOG_ERROR("MPCClientAgent::create Init system task error !");
        goto ERR;
    }
    if(task->init(std::tr1::bind(&MPCClientAgent::after_init_done , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2)) < 0)
    {
        LOG_ERROR("MPCClientAgent::init system task init error !");
        delete task;
        goto ERR;
    }

    std::cerr<<"---------------init debug--------------"<<std::endl;
    std::cerr<<"create init system task success !"<<std::endl;
    m_init_task = task;

    return 0;

INPUT_ERR :
    CSTCPAgent::reply_request(cmd_ack , MPC_INPUT_ERROR , 
            0 , 0 , out_data);
    return 0;

ERR :
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 
            0 , 0 , out_data);

    return -1;
}

//函数输出-1表示出错
//输出-2表示MPC输入出错
//输出0表示正常
int MPCClientAgent::init_globle_hash_rule(RULE_TYPE type , const vector<cstore::IP_scope> &ip_scope , int mod , int duplicate_num , std::vector<uint32_t> &init_ip_list)
{
    cstore::IP_scope scope_item;
    uint32_t  start_ip = INVALID_IP;
    uint32_t  serial_ip_num = 0;
    int last_ip_num = 0;
    std::string ip_temp;
    uint32_t  ip_num;

    //将所有的IP保存在list中
    for(uint32_t i = 0 ; i < ip_scope.size() ; ++ i)
    {
        scope_item = ip_scope[i];
        start_ip = scope_item.start_ip();
        serial_ip_num = scope_item.serial_num();

        //先将这些连续IP地址的第一个地址转换成string类型
        if(int_to_string_ip(start_ip , ip_temp) < 0)
        {
            LOG_ERROR("MPCClientAgent::int to string error !");
            return -1 ;
        }
        //获取这个连续的IP地址第一个地址的最后一项的内容
        last_ip_num = get_last_ip_num(ip_temp);
        //如果这一段连续的IP地址不符合规范，例如192.168.1.254开始的10个IP
        if(last_ip_num < 0 || last_ip_num + serial_ip_num >= 255)
        {
            LOG_ERROR("MPCClientAgent::Invalid Input : Start ip adderss : "
            + ip_temp + " Serial ip number : " + int_to_str(serial_ip_num));
            return -2 ;
        }
        //将第一个IP的最后一项递增再转换成int类型的IP地址
        for(uint32_t j = 0 ; j < serial_ip_num ; ++ j)
        {
            append_new_ip(ip_temp , last_ip_num + j);
            if(string_to_int_ip(ip_temp , ip_num) < 0)
            {
                LOG_ERROR("MPCClientAgent::string to int error !");
                return -1; ;
            }
            init_ip_list.push_back(ip_num);
        }
    }
    //如果指定的节点数目小于副本数，因为一个副本不能放在同一个节点
    //因此这时候返回出错...
    int node_num = init_ip_list.size();
    if(duplicate_num > node_num)
    {
        LOG_ERROR("MPCClientAgent::Invalid Input : duplicate number : " 
                + int_to_str(duplicate_num) + " Node number : " 
                + int_to_str(node_num));
        return -2;
    }
    
    //做个翻转，用于测试
    std::vector<uint32_t> init_ip_list_back(init_ip_list.rbegin() , init_ip_list.rend());
    init_ip_list.assign(init_ip_list_back.begin() , init_ip_list_back.end());

    return 0;
}

int MPCClientAgent::do_migrate_bucket(RULE_TYPE type , InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t  cmd_ack = 0;
    std::string out_data;
    std::string input_data(req.ioBuf , header.length);
    struct Migration_Info info;
    memset(&info , 0 , sizeof(info));
    MigrationTask *migrate_task = NULL;

    cstore::pb_MSG_MPC_CS_MU_MIGRATE_BUCKET   in_mu_migrate;
    cstore::pb_MSG_MPC_CS_SU_MIGRATE_BUCKET   in_su_migrate;
    if(MU_RULER == type)
    {
        cmd_ack = MSG_MPC_CS_MU_MIGRATE_BUCKET_ACK;
        if(!in_mu_migrate.ParseFromString(input_data))
        {
            LOG_ERROR("MPCClientAgent::protobuf parse error !");
            goto ERR;
        }
        std::cerr<<"-------MU migration---------"<<std::endl;
        std::cerr<<in_mu_migrate.DebugString()<<std::endl;

        info.src_ip = in_mu_migrate.source_ip();
        info.dest_ip = in_mu_migrate.destination_ip();
        info.bucket_nr = in_mu_migrate.bucket_num();
    }
    else
    {
        cmd_ack = MSG_MPC_CS_SU_MIGRATE_BUCKET_ACK;
        if(!in_su_migrate.ParseFromString(input_data))
        {
            LOG_ERROR("MPCClientAgent::protobuf parse error !");
            goto ERR;
        }

        std::cerr<<"-------SU migration---------"<<std::endl;
        std::cerr<<in_su_migrate.DebugString()<<std::endl;



        info.src_ip = in_su_migrate.source_ip();
        info.dest_ip = in_su_migrate.destination_ip();
        info.bucket_nr = in_su_migrate.bucket_num();
    }

    //如果当前系统正在处于不能执行这两个地址的桶迁移，回复MPC，或者暂时阻塞
    
    if(!TaskManager::getInstance()->can_exec_migration(type , info.src_ip , info.dest_ip))
    {
        LOG_EVENT("Now can not execuate migrate between IP " 
                + int_to_str(info.src_ip) + " AND IP " 
                + int_to_str(info.dest_ip));

        goto BUSY;
    }

    if(info.bucket_nr >= RulerManager::getInstance()->get_bucket_number(type))
    {
        LOG_ERROR("MPCClientAgent::migrate bucket number : " + int_to_str(info.bucket_nr));
        goto INPUT_ERR ;
    }

    //判断需要迁移的桶号和对应的IP地址是否相对应。两个IP必须与CS保持心跳
    //需要迁移的桶的规则表必须有源节点的IP地址
    if(!HBManager::getInstance()->IP_is_valid_for_migration(type , info))
    {
        LOG_ERROR("MPCClientAgent::IP is not a node of System ! IP : " 
                + int_to_str(info.src_ip) + " AND IP " 
                + int_to_str(info.dest_ip));

        goto INPUT_ERR ;
    }

    //CREATE migration task to deal with this request !
    //设置回调函数为do_migrate_callback
    migrate_task = new MigrationTask(CSTCPAgent::m_index , type , info);
    if(NULL == migrate_task)
    {
        LOG_ERROR("MPCClientAgent::create MigrationTask error !");
        goto ERR ;
    }
    if(migrate_task->init(std::tr1::bind(&MPCClientAgent::do_migrate_callback , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3)) < 0)
    {
        LOG_ERROR("MPCClientAgent::init Migration task error !");
        delete migrate_task;
        migrate_task = NULL;
        goto ERR ;
    }

    m_migrate_task = migrate_task;

//    do_migrate_callback(true , type , info);

    return 0;
     
ERR :
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 
            0 , 0 , out_data);
    return -1;

INPUT_ERR :
    CSTCPAgent::reply_request(cmd_ack , MPC_INPUT_ERROR , 
            0 , 0 , out_data);
    return 0;

BUSY :
    CSTCPAgent::reply_request(cmd_ack , CS_IS_BUSY , 
            0 , 0 , out_data);
    return 0;
}

int MPCClientAgent::do_extent_bucket(RULE_TYPE type , InReq &req)
{
    MsgHeader header = req.m_msgHeader;
    uint32_t  cmd_ack = 0;
    uint32_t current_mod = 0;
    std::string out_data;
    std::string input_data(req.ioBuf , header.length);
    struct Extent_Info info;
    memset(&info , 0 , sizeof(info));
    ExtentTask *extent_task = NULL;

    cstore::pb_MSG_MPC_CS_MU_EXTENT_BUCKET  in_mu_extent;
    cstore::pb_MSG_MPC_CS_SU_EXTENT_BUCKET  in_su_extent;

    if(MU_RULER == type)
    {
        cmd_ack = MSG_MPC_CS_MU_EXTENT_BUCKET_ACK;
        if(!in_mu_extent.ParseFromString(input_data))
        {
            LOG_ERROR("MPCClientAgent:protobuf parse error !");
            goto ERR;
        }
        std::cerr<<"-------MU Extention---------"<<std::endl;
        std::cerr<<in_mu_extent.DebugString()<<std::endl;


        info.new_mod = in_mu_extent.new_mod();
    }
    else
    {
        cmd_ack = MSG_MPC_CS_SU_EXTENT_BUCKET_ACK;
        if(!in_su_extent.ParseFromString(input_data))
        {
            LOG_ERROR("MPCClientAgent:protobuf parse error !");
            goto ERR;
        }
        std::cerr<<"-------SU Extention---------"<<std::endl;
        std::cerr<<in_su_extent.DebugString()<<std::endl;

        info.new_mod = in_su_extent.new_mod();
    }

    if(!TaskManager::getInstance()->can_exec_extent(type))
    {
        LOG_EVENT("Now can not execuate Extent operation In CSTORE system ! ");
        goto BUSY ;
    }
    //桶迁移必须将版本号增加1
    current_mod = RulerManager::getInstance()->get_current_mod(type);
    info.new_mod = current_mod + 1;
    if(info.new_mod != current_mod + 1)
    {
        LOG_ERROR("MPCClientAgent::current mod : " + int_to_str(current_mod) 
                + " new mod : " + int_to_str(info.new_mod));
        goto INPUT_ERR ;
    }

    //创建extent task完成桶迁移操作，最后修改规则，设置回调函数
    extent_task = TaskManager::getInstance()->create_extent_task(type , CSTCPAgent::m_index , info);
    if(NULL == extent_task)
    {
        LOG_ERROR("MPCClientAgent::create new Extent task error !");
        goto ERR ;
    } 
    if(extent_task->init(std::tr1::bind(&MPCClientAgent::do_extent_callback , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 , 
                    std::tr1::placeholders::_3)) < 0)
    {
        LOG_ERROR("MPCClientAgent::Init extent task errro !");
        TaskManager::getInstance()->delete_extent_task(type);
        goto ERR;
    }
    
//    do_extent_callback(true , type , info);
    return 0;
ERR :
    CSTCPAgent::reply_request(cmd_ack , CS_ERROR , 
            0 , 0 , out_data);
    return -1;

INPUT_ERR :
    CSTCPAgent::reply_request(cmd_ack , MPC_INPUT_ERROR , 
            0 , 0 , out_data);
    return 0;

BUSY :
    CSTCPAgent::reply_request(cmd_ack , CS_IS_BUSY , 
            0 , 0 , out_data);
    return 0;

}

//规则的改变在MigrationTask中完成！
void MPCClientAgent::do_migrate_callback(bool ret , RULE_TYPE type , Migration_Info info)
{
    uint32_t para1 = 0;
    uint32_t cmd = 0;
    std::string out_data;
    //无论执行成功或者失败，0都需要删除当前的Task
    if(m_migrate_task != NULL)
    {
        delete m_migrate_task ;
        m_migrate_task = NULL;
    }

    //执行成功,通过para1参数标志迁移执行成功或者失败
    if(ret)
    {
        para1 = MPC_CS_MIGRATE_SUCCESS;
    }
    //如果执行失败，则什么操作都不需要进行，直接回复MPC
    else
        para1 = MPC_CS_MIGRATE_FAILED;

    if(MU_RULER == type)
        cmd = MSG_MPC_CS_MU_MIGRATE_BUCKET_ACK;
    else
        cmd = MSG_MPC_CS_SU_MIGRATE_BUCKET_ACK;
    
    CSTCPAgent::reply_request(cmd , CS_OK , para1 , 0 , out_data);
}

//规则的改变在ExtentTask中完成！
void MPCClientAgent::do_extent_callback(bool ret , RULE_TYPE type , Extent_Info info)
{
    uint32_t para1 = 0;
    uint32_t cmd = 0;
    std::string out_data;

    ExtentTask *task = TaskManager::getInstance()->get_extent_task(type);
    if(task != NULL && task->can_recycler_task())
    {
        TaskManager::getInstance()->delete_extent_task(type);
    }

    if(ret)
    {
        para1 = MPC_CS_EXTENT_SUCCESS;
    }
    else
        para1 = MPC_CS_EXTENT_FAILED;

    if(MU_RULER == type)
        cmd = MSG_MPC_CS_MU_EXTENT_BUCKET_ACK;
    else
        cmd = MSG_MPC_CS_SU_EXTENT_BUCKET_ACK;

    CSTCPAgent::reply_request(cmd , CS_OK , para1 , 0 , out_data);
}

void MPCClientAgent::after_init_done(bool ret , RULE_TYPE type)
{
    uint32_t err = 0;
    uint32_t cmd = 0;
    std::string out;
    if(ret)
        err = CS_OK;
    else
        err = CS_ERROR;

    if(MU_RULER == type)
        cmd = MSG_MPC_CS_MU_INIT_HASH_ACK;
    else 
        cmd = MSG_MPC_CS_SU_INIT_HASH_ACK;

    CSTCPAgent::reply_request(cmd , err , 0 , 0 , out);
}

