/*
 * =====================================================================================
 *
 *       Filename:  Init_rule.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/15/12 10:24:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "AgentRegister.h"
#include "Init_rule.h"
#include "CSConfiguration.h"
#include "CS_Epoll.h"

#include "Log_warp.h"
#include "Lib_tool.h"
#include "../common/protocol/protocol.h"
#include "Rule_file_interface.h"

const char *rule_prefix = "select * from ";
const char *rule_mid = " where bucket = ";

const char *order_prefix = "select * from ";
const char *order_mid = " where version = ";

const char *rows_prefix = "select count(*) from ";
//用来验证数据库是否出现错误
const char *ext_info_prefix = "select max(new_mod) from (select new_mod from ";
const char *ext_info_mid = ") as tp";

const char *select_rule_prefix = "select * from ";
const char *select_rule_mid = " where (bucket = ?)";

const char *select_order_prefix = "select * from ";

const char *show_table_prefix = "show tables like ";

RuleInit::RuleInit()
    :m_is_rs(true) , 
    m_is_cs(true) ,
    m_mu_rule_agent_index(0) , 
    m_su_rule_agent_index(0)
{

}

RuleInit::~RuleInit()
{
    if(m_mu_rule_agent_index != 0)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_mu_rule_agent_index);
        m_mu_rule_agent_index = 0;
    }
    if(m_su_rule_agent_index != 0)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_su_rule_agent_index);
        m_su_rule_agent_index = 0;
    }
}

//根据模块的类型执行一些必要的初始化工作
//如果这仅仅是一个CS模块，那么CS初始化的时候应该读取数据库的信息，也就是做缓存。
//并且需要创建监听套接口，监听RS拉取最新规则的请求。
//如果这仅仅是一个RS模块，那么需要向CS拉取规则
//如果这既是一个RS也是CS模块，只需要读取数据库的初始化信息。
//任何出错都会调用回调函数，通知启动是否成功
int RuleInit::init_rule()
{
    int ret = -1;
    SocketAddress listen_addr;

    //如果这既是一个RS模块，也是一个CS模块
    //RS的规则全由CS保持，CS完全兼并RS的功能
    //RS只向外提供一个获取规则的端口就可以了
    if(m_is_rs && m_is_cs)
    {
        ret = get_whole_rule_from_db();
        if(ret < 0)
        {
            LOG_ERROR("RunControl::CS Get Whole Rule error !");
            return -1;
        }
        
       return 0; 
    }

    //如果这只是一个CS模块，从数据库获取规则信息
    if(m_is_cs && !m_is_rs)
    {
        ret = get_whole_rule_from_db();
        if(ret < 0)
        {
            LOG_ERROR("RunControl::CS Get Whole Rule error !");
            return -1;
        }
        return 0;
    }

    return 0;
}

#define DBP DBControl::getInstance()
extern const char *op_types[];

int RuleInit::get_whole_rule_from_db()
{
    Connection_T conn = DBP->create_connection();
    if(NULL == conn)
    {
        LOG_ERROR("RuleInit::create connection error !");
        return -1;
    }

    if(init_mu_rule(conn) < 0)
    {
        LOG_ERROR("RuleInit::init mu rule from database to memory error !");
        goto ERR;
    }
    if(init_su_rule(conn) < 0)
    {
        LOG_ERROR("RuleInit::init su rule from database to memory error !");
        goto ERR;
    }

    DBP->close_connection(conn);
    conn = NULL;
    return 0;

ERR : 
    DBP->close_connection(conn);
    conn = NULL;
    return -1;
}

int RuleInit::get_bucket_number(Connection_T conn , RULE_TYPE type)
{
    std::string get_rows_sql ;
    if(MU_RULER == type)
        get_rows_sql = rows_prefix + CSConfiguration::getInstance()->mu_rule_table;
    else
        get_rows_sql = rows_prefix + CSConfiguration::getInstance()->su_rule_table;

    ResultSet_T rets = NULL;
    int rows = 0;
    if((!(rets = DBP->execute_query(conn , get_rows_sql))) || (!ResultSet_next(rets)) 
            || ((rows = DBP->get_int_result(rets , 1)) < 0))
    {
        LOG_ERROR("RuleInit::get sum bucket number error : " + get_rows_sql);
        return -1;
    }

    return rows;    
}

int RuleInit::get_mod_from_db(Connection_T conn , RULE_TYPE type)
{
    std::string get_mod_sql;

    get_mod_sql = ext_info_prefix + DBP->get_order_table_name(type)
            + " where command = \"" EXTENSION_COMMAND + "\"" + ext_info_mid;

    ResultSet_T rets = NULL;
    int rows = 0;
    if(!(rets = DBP->execute_query(conn , get_mod_sql))) 
    {
        LOG_ERROR("RuleInit::get current mod from database error : " + get_mod_sql);
        return -1;
    }
    if(!ResultSet_next(rets))
    {
        LOG_EVENT("RuleInit::Extent infomation empty !");
        return 0;
    }
    if((rows = DBP->get_int_result(rets , 1)) < 0)
    {
        LOG_ERROR("RuleInit::get Extent infomation result error !");
        return -1;
    }

    return rows;
}

int RuleInit::generate_all_rule(Connection_T conn , RULE_TYPE type , int bucket_num)
{
//    int bucket_num = cs_pow(mod);
    std::string rule_table = DBP->get_rule_table_name(type);

    std::string select_sql = select_rule_prefix + rule_table + select_rule_mid;
    PreparedStatement_T pre = DBP->prepare_execute(conn , select_sql);
    if(NULL == pre)
    {
        LOG_ERROR("RuleInit::Prepare statement query sql error : " + select_sql);
        return -1;
    }

    ResultSet_T rets = NULL;
    std::string temp_ip;
    uint32_t ip = 0;
    struct HashRule rule;
    for(int i = 0 ; i < bucket_num ; ++ i)
    {
        memset(&rule , 0 , sizeof(rule));
        if(DBP->bind_int_parameter(pre , 1 , i) < 0)
        {
            LOG_ERROR("RuleInit::bind int parameter to bucket error ! bucket : " 
                    + int_to_str(i));
            return -1;
        }
        if(!(rets = DBP->prepare_execute_query(pre)) || (!ResultSet_next(rets)))
        {
            LOG_ERROR("RuleInit::get result error ! bucket : " + int_to_str(i));
            return -1;
        }
        //只获得IP，因此需要从第二个数开始
        for(int j = 0 ; j < MAX_IP_NUM ; ++ j)
        {
            if(DBP->get_string_result(rets , j + 2 , temp_ip) < 0)
            {
                LOG_ERROR("RuleInit::Get string result of index " + int_to_str(j + 2) + "error !");
                return -1;
            }
            //如果这是一个无效的IP
            if(!temp_ip.compare(INVALID_IP_STR))
            {
                ip = INVALID_IP;
            }
            else 
            {
                if(string_to_int_ip(temp_ip , ip) < 0)
                {
                    LOG_ERROR("RuleInit::Change string ip to int error : " + temp_ip);
                    return -1;
                }
            }
            rule.ip[j] = ip;
        }
        rule.bucket_nr = i;
        RulerManager::getInstance()->add_new_hash_item(rule , type);
    }
    int mod = cs_log(bucket_num);
    if(mod == -1)
        mod = 0;

    RulerManager::getInstance()->set_current_mod(type , mod) ;
        
    return 0;
}

int RuleInit::generate_all_order(Connection_T conn , RULE_TYPE type)
{
    std::string table_name = DBP->get_order_table_name(type);
    std::string order_sql = select_order_prefix + table_name;

    std::string cmd;
    std::string src_ip , dest_ip;
    int bucket , mod , version;

    ResultSet_T rets = DBP->execute_query(conn , order_sql);
    if(rets == NULL)
    {
        LOG_ERROR("RuleInit::execute query order table sql error : " + order_sql);
        return -1;
    }
    struct OrderRule order;
    int ver_n = 0;
    //循环查找每一个结果
    while(ResultSet_next(rets))
    {
        memset(&order , 0 , sizeof(order));
        if((version = DBP->get_int_result(rets , 1)) < 0)
        {
            LOG_ERROR("RuleInit::get 1 int result of order result error !");
            return -1;
        }
        if(DBP->get_string_result(rets , 2 , cmd) < 0)
        {
            LOG_ERROR("RuleInit::get 2 string result of order select result error !");
            return -1;
        }
        //如果是桶迁移需要将new_mod那一列当成迁移的桶号
        if(!cmd.compare(MIGRATION_COMMAND))
        {
            if(((bucket = DBP->get_int_result(rets , 3)) < 0) || 
                (DBP->get_string_result(rets , 4 , src_ip) < 0) || 
                (DBP->get_string_result(rets , 5 , dest_ip) < 0))
            {
                LOG_ERROR("RuleInit::get other result of migration info error !");
                return -1;
            }
            uint32_t ip_src , ip_dest;
            if((string_to_int_ip(src_ip , ip_src) < 0) || 
               (string_to_int_ip(dest_ip , ip_dest) < 0))
            {
                LOG_ERROR("RuleInit::change ip to string error : " + src_ip + " and " + dest_ip);
                return -1;
            }
            struct Migration_Info info;
            info.bucket_nr = bucket;
            info.src_ip = ip_src;
            info.dest_ip = ip_dest;
            order.cmd = MIGRATION;
            order.order.migration = info;
        }
        else if(!cmd.compare(EXTENSION_COMMAND))
        {
            if((mod = DBP->get_int_result(rets , 3)) < 0)
            {
                LOG_ERROR("RuleInit::get 3 mod result of order result error !");
                return -1;
            }
            struct Extent_Info info;
            info.new_mod = mod;
            order.cmd = EXTENT;
            order.order.extent = info;
        }
        else
        {
            LOG_ERROR("exit here !");
            exit(-1);
        }
        order.version = version;
        RulerManager::getInstance()->add_new_order_item(order , type);
        ++ ver_n;
    }
    RulerManager::getInstance()->set_current_version(type , ver_n);
    std::cerr<<(type == MU_RULER ? "MU " : "SU")<<"Current version "<<ver_n<<std::endl;

    return 0;
}

int RuleInit::init_mu_rule(Connection_T conn)
{
    RulerManager::getInstance()->init_system_rule(MU_RULER);
    if(!system_if_initialized(conn , MU_RULER))
    {
        LOG_INIT("!!!MU IS NOT BEEN INITIALIZED!!!");
        return 0;
    }

    int bucket_num = get_bucket_number(conn , MU_RULER);
    int mod = get_mod_from_db(conn , MU_RULER);
    if((bucket_num < 0) || (mod < 0))
    {
        LOG_ERROR("RuleInit::get bucket number or get mod error when init mu rule !");
        return -1;
    }
    
    std::cerr<<"MU bucket number : "<<bucket_num<<" and current mod : "<<mod<<std::endl;
    
    if((this->generate_all_rule(conn , MU_RULER , bucket_num) < 0) || 
        (this->generate_all_order(conn , MU_RULER) < 0))
    {
        LOG_ERROR("RuleInit::init mu rule table or order table in memory error !");
        RulerManager::getInstance()->init_system_rule(MU_RULER);
        return -1;
    }
    return 0;
}

int RuleInit::init_su_rule(Connection_T conn)
{
    RulerManager::getInstance()->init_system_rule(SU_RULER);
    if(!system_if_initialized(conn , SU_RULER))
    {
        LOG_INIT("!!!SU IS NOT BEEN INITIALIZED!!!");
        return 0;
    }

    int bucket_num = get_bucket_number(conn , SU_RULER);
    int mod = get_mod_from_db(conn , SU_RULER);
    if((bucket_num < 0) || (mod < 0))
    {
        LOG_ERROR("RuleInit::get bucket number or get mod error when init su rule !");
        return -1;
    }
    
    std::cerr<<"SU bucket number : "<<bucket_num<<" and current mod : "<<mod<<std::endl;
    
    if((this->generate_all_rule(conn , SU_RULER , bucket_num) < 0) || 
        (this->generate_all_order(conn , SU_RULER) < 0))
    {
        LOG_ERROR("RuleInit::init su rule table or order table in memory error !");
        RulerManager::getInstance()->init_system_rule(SU_RULER);
        return -1;
    }
    return 0;
}

//查看当前数据库是否存在规则表的桶，如果存在说明已初始化，否则没有初始化!
bool RuleInit::system_if_initialized(Connection_T conn , RULE_TYPE type)
{
    std::string rule_sql = std::string(show_table_prefix) + "\"" + DBP->get_rule_table_name(type) + "\"";

    ResultSet_T ret = DBP->execute_query(conn , rule_sql);
    if(NULL == ret)
    {
        LOG_ERROR("RuleInit::execute query rule table error : " + rule_sql);
        return false;
    }
    
    //如果查询结果为空
    if(!ResultSet_next(ret))
        return false;
    else 
        return true;
}




/*  
//下面都是为但CS准备的，暂时不考虑!
int RuleInit::get_whole_rule_from_cs()
{
    if(get_rule_from_cs(MU_RULER) < 0)
    {
        LOG_ERROR("RunControl::Get MU rule error !");
        return -1;
    }
    if(get_rule_from_cs(SU_RULER) < 0)
    {
        LOG_ERROR("RunControl::Get SU rule error !");
        return -1;
    }

    return 0;
}

int RuleInit::get_rule_from_cs(RULE_TYPE type)
{
    std::string req_data;
    std::string out_string;
    int ret = -1;
    uint32_t agent_index = 0;

    //请求全部的规则信息
    MsgHeader header;
    memset(&header , 0 , sizeof(header));
    if(type == MU_RULER)
    {
        header.cmd = MSG_RS_CS_UPDATE_ALL_MU_HASH;
    }
    else
    {
        header.cmd = MSG_RS_CS_UPDATE_ALL_SU_HASH;
    }
    header.length = 0;

    if(type == MU_RULER)
    {
        cstore::pb_MSG_RS_CS_UPDATE_ALL_MU_HASH out_pb;
        //初始化，版本为0
        out_pb.set_local_version(RulerManager::getInstance()->get_current_version(type));
        if((!out_pb.IsInitialized()) || (!out_pb.SerializeToString(&out_string)))
        {
            LOG_ERROR("RuleInit::protobuf is error when get mu rule !");
            return -1;
        }

        LOG_EVENT("Init RS rule : cstore::pb_MSG_RS_CS_UPDATE_ALL_MU_HASH : " 
                + out_pb.DebugString());
        
    }
    if(type == SU_RULER)
    {
        cstore::pb_MSG_RS_CS_UPDATE_ALL_SU_HASH out_pb;

        //初始化，版本为0
        out_pb.set_local_version(RulerManager::getInstance()->get_current_version(type));
        if((!out_pb.IsInitialized()) || (!out_pb.SerializeToString(&out_string)))
        {
            LOG_ERROR("RuleInit::protobuf is error when get su rule !");
            return -1;
        }

        LOG_EVENT("Init RS rule : cstore::pb_MSG_RS_CS_UPDATE_ALL_SU_HASH : " 
                + out_pb.DebugString());
        
    }

    SocketAddress oppo_addr(CSConfiguration::getInstance()->rs_cs_ip.c_str() , 
            CSConfiguration::getInstance()->rs_cs_port);

    SCAgent *agent = AgentRegister::getInstance()->create_SC_agent(oppo_addr 
             CSEpoll::getInstance());

    if(NULL == agent)
    {
        LOG_ERROR("RuleInit::create SC agent error !");
        return -1;
    }
    agent_index = agent->get_agent_index();
    //将请求数据全部封装在req_data中
    agent->do_package(header , out_string , req_data);
    
    ret = agent->init(std::tr1::bind(&RuleInit::done_get_rule_from_cs , 
                    this , 
                    std::tr1::placeholders::_1 , 
                    std::tr1::placeholders::_2 ,
                    std::tr1::placeholders::_3) , req_data , NULL);
    if(ret < 0)
    {
        LOG_ERROR("RuleInit::SCAgent Init error !");
        AgentRegister::getInstance()->delete_SC_agent(agent_index);
        return -1;
    }

    if(MU_RULER == type)
        m_mu_rule_agent_index = agent->get_agent_index();
    else 
        m_su_rule_agent_index = agent->get_agent_index();

    return 0;
}

void RuleInit::done_get_rule_from_cs(void *req , RET_TYPE type , void *data)
{
    static bool get_su_rule = false ;
    static bool get_mu_rule = false ;
    static int ret_counter = 0;

    InReq *rule_req = (InReq *)req;
    uint32_t cmd = 0;

    ++ ret_counter;
    if(UNKOWN_ERROR == type)
    {
        LOG_ERROR("RuleInit::get rule from cs error : unknow error !");
        goto END ;
    }
    else if(CONNECT_ERROR == type)
    {
        LOG_ERROR("RuleInit::get rule from cs error : connect error !");
        goto END ;
    }
    else if(WRITE_ERROR == type)
    {
        LOG_ERROR("RuleInit::get rule from cs error : write error !");
        goto END ;
    }

    if(rule_req != NULL)
    {
        MsgHeader header = rule_req->m_msgHeader;
        cmd = header.cmd;
    }
    else 
        goto END ;

    switch(cmd)
    {
        case MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK :
        {
            get_rule_ack_from_cs(MU_RULER , rule_req);
            get_mu_rule = true ;
            if(true == get_su_rule)
            {
                goto SUCCESS;
            }
            break ;
        }
        case MSG_RS_CS_UPDATE_ALL_SU_HASH_ACK :
        {
            get_rule_ack_from_cs(SU_RULER , rule_req);
            get_su_rule = true;
            if(true == get_mu_rule)
            {
                goto SUCCESS;
            }
            break ;
        }

        default :
            LOG_ERROR("RuleInit::Get Unknow request ack type when get rule ! ACK cmd : " + int_to_str(cmd));
            goto END;
    }

SUCCESS :
    do_call_back(true);
    return ;

END :
    if(!(get_mu_rule && get_su_rule))
        return ;

    //需要在start_rs中回收这两个agent
    AgentRegister::getInstance()->delete_SC_agent(m_su_rule_agent_index);
    m_su_rule_agent_index = 0;

    AgentRegister::getInstance()->delete_SC_agent(m_mu_rule_agent_index);
    m_mu_rule_agent_index = 0;

    if((get_mu_rule && get_su_rule))
        do_call_back(false);
}

int RuleInit::get_rule_ack_from_cs(RULE_TYPE type , InReq *req)
{
    //无负载数据
    if(0 == req->m_msgHeader.length)
        return 0;

    OP_CMD   order_cmd;
    uint32_t bucket_nr = 0;
    uint32_t *ip_array = NULL;
    uint32_t bucket_map_size;

    uint32_t version = 0;
    uint32_t cmd = 0;
    std::string content;

    string proto_input(req->ioBuf , req->m_msgHeader.length);
    cstore::Map_Item             bucket_map;
    cstore::Sys_Order            rule_order;

    ip_array = new uint32_t[MAX_IP_NUM];
    if(type == MU_RULER)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_mu_rule_agent_index);
        m_mu_rule_agent_index = 0;

        cstore::pb_MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK in_pb;

        if(!in_pb.ParseFromString(proto_input))
        {
            LOG_ERROR("RuleInit::parse from protobuf error after get mu rule !");
            goto FREE;
        }

        LOG_EVENT("Get RS init Rule : MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK : " + in_pb.DebugString());

        for(int i = 0 ; i < in_pb.map_bucket_size() ; ++ i)
        {
            bucket_map = in_pb.map_bucket(i);
            bucket_nr = bucket_map.bucket_num();
            bucket_map_size = bucket_map.module_ip_size();

            for(uint32_t j = 0 ; j < bucket_map_size && j < MAX_IP_NUM ; ++ j)
                ip_array[j] = bucket_map.module_ip(j);

            RulerManager::getInstance()->set_hash_rule(bucket_nr , ip_array , 3 , type);
        } 
        for(int i = 0 ; i < in_pb.sys_order_size() ; ++ i)
        {
            rule_order = in_pb.sys_order(i);
            version = rule_order.hash_version();
            cmd = rule_order.order_header();

            if(MSG_MPC_CS_MU_MIGRATE_BUCKET == cmd)
                order_cmd = MIGRATION;
            else if(MSG_MPC_CS_MU_EXTENT_BUCKET == cmd)
                order_cmd = EXTENT;
            else 
            {
                LOG_ERROR("RuleInit::Invalid mu order header !");
                goto FREE;
            }
            content = rule_order.order_content();

            RulerManager::getInstance()->set_order_rule(version , order_cmd , content , type);
        }
    }
    if(SU_RULER == type)
    {
        AgentRegister::getInstance()->delete_SC_agent(m_su_rule_agent_index);
        m_su_rule_agent_index = 0;

        cstore::pb_MSG_RS_CS_UPDATE_ALL_SU_HASH_ACK in_pb;

        if(!in_pb.ParseFromString(proto_input))
        {
            LOG_ERROR("RuleInit::parse from protobuf error after get mu rule !");
            goto FREE;
        }

        LOG_EVENT("Get RS init Rule : MSG_RS_CS_UPDATE_ALL_MU_HASH_ACK : " + in_pb.DebugString());

        for(int i = 0 ; i < in_pb.map_bucket_size() ; ++ i)
        {
            bucket_map = in_pb.map_bucket(i);
            bucket_nr = bucket_map.bucket_num();
            bucket_map_size = bucket_map.module_ip_size();

            for(uint32_t j = 0 ; j < bucket_map_size && j < MAX_IP_NUM ; ++ j)

            RulerManager::getInstance()->set_hash_rule(bucket_nr , ip_array , 3 , type);
        } 
        for(int i = 0 ; i < in_pb.sys_order_size() ; ++ i)
        {
            rule_order = in_pb.sys_order(i);
            version = rule_order.hash_version();
            cmd = rule_order.order_header();

            if(MSG_MPC_CS_SU_MIGRATE_BUCKET == cmd)
                order_cmd = MIGRATION;
            else if(MSG_MPC_CS_SU_EXTENT_BUCKET == cmd)
                order_cmd = EXTENT;
            else 
            {
                LOG_ERROR("RuleInit::Invalid su order header !");
                goto FREE;
            }

            std::string content = rule_order.order_content();

            RulerManager::getInstance()->set_order_rule(version , order_cmd , content , type);
        }
    }

    if(ip_array != NULL)
    {
        delete ip_array;
        ip_array = NULL;
    }
    return 0;

FREE :
    if(ip_array != NULL)
    {
        delete ip_array;
        ip_array = NULL;
    }
    return -1;
}
*/
