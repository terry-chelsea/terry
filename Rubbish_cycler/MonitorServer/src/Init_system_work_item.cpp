/*
 * =====================================================================================
 *
 *       Filename:  CS_DB_operation.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/07/13 18:49:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "Init_system_work_item.h"
#include "CSConfiguration.h"
#include "Log_warp.h"
#include "Lib_tool.h"

//删除表的时候需要加上是否存在，如果不存在就不执行
const char *drop_prefix = "drop table if exists ";
const char *create_prefix = "create table ";
//创建规则表的各个字段的定义
const char *rule_table_attr = "(bucket int(10) unsigned not null primary key , ip0 varchar(32) not null , ip1 varchar(32) , ip2 varchar(32) , ip3 varchar(32) , ip4 varchar(32))";

//创建版本表的各个字段的定义
const char *order_table_attr = "(version int(10) unsigned primary key auto_increment , command varchar(10) , new_mod int unsigned , source_ip varchar(32) , dest_ip varchar(32))";
//!!!new_mod字段在init_rule的时候使用了，如果修改需要一起修改!!!

//创建操作历史记录表的各个字段的定义 0表示正在进行 ， -1表示失败 ， 1表示成功
const char *history_table_attr = "(id int(10) unsigned primary key auto_increment , type varchar(20) not null , protoinfo varchar(256) , state smallint(4) , addition varchar(32) , start datetime , end datetime)";
//!!!bucket字段在init_rule的时候使用了，如果修改需要一起修改!!!

const char *insert_rule_prefix = "insert into ";
const char *insert_rule_prepare = " values (? , ? , ? , ? , ? , ?)";
const int MAX_LINE = 1024;
extern const char *op_types[];

DBControl* DBP = DBControl::getInstance();

int InitSystemWorkItem::process()
{
    std::string type_str = m_rule_type == MU_RULER ? "MU" : "SU";
    if(init_cstore_database(m_rule_type , m_init_info.rule_table , m_init_info.dup , m_init_info.mod) < 0)
    {
        LOG_ERROR("InitSystemWorkItem::init mu rule to database error !");
        m_exec_result = false;
        return -1;
    }

    m_exec_result = true;
    return 0;
}

int InitSystemWorkItem::init_tables(Connection_T conn_p , RULE_TYPE type)
{
    std::string rule_table = DBP->get_rule_table_name(type);
    std::string order_table = DBP->get_order_table_name(type);
    std::string history_table = CSConfiguration::getInstance()->history_table;

    //在执行初始化操作之前首先清空规则表和版本表，首先删除然后创建
    std::string drop_sql = drop_prefix + rule_table;
    std::string create_sql = create_prefix + rule_table + rule_table_attr;
    if((DBP->execute_operation(conn_p , drop_sql) < 0) || 
            (DBP->execute_operation(conn_p , create_sql) < 0))
    {
        LOG_ERROR("DBControl::drop or create rule table error ! drop sql : " + drop_sql 
                + " and create sql : " + create_sql);
        goto ERR ;
    }
    
    drop_sql = drop_prefix + order_table;
    create_sql = create_prefix + order_table + order_table_attr;
    if((DBP->execute_operation(conn_p , drop_sql) < 0) || 
            (DBP->execute_operation(conn_p , create_sql) < 0))
    {
        LOG_ERROR("DBControl::drop or create order table error ! drop sql : " + drop_sql 
                + " and create sql : " + create_sql);
        goto ERR1 ;
    }
    
    if(this->init_operation_table(conn_p) < 0)
    {
        LOG_ERROR("DBControl::drop or create info table error !");
        goto ERR2 ;
    }

    return 0;
    //因为创建操作在后面，因此只需要drop之前创建的table就可以了。
ERR2 :
    drop_sql = drop_prefix + order_table;
    DBP->execute_operation(conn_p , drop_sql);

ERR1 :
    drop_sql = drop_prefix + rule_table;
    DBP->execute_operation(conn_p , drop_sql);

ERR :
    return -1;
}

int InitSystemWorkItem::generate_every_bucket_rule(PreparedStatement_T pre , struct HashRule rule)
{
    bool err = false;
    int index = 0;
    //需要使用数组，因为预处理的参数是在真正执行的时候才读取的，而不是绑定参数的时候复制
    std::string ip_str[MAX_IP_NUM];
    if(DBP->bind_int_parameter(pre , 1 , rule.bucket_nr) < 0)
    {
        err = true;
        goto OUT;
    }

    for(int i = 0 , index = 2 ; i < MAX_IP_NUM ; ++ i , ++ index)
    {
        uint32_t ip = rule.ip[i];
        if(ip != INVALID_IP)
        {
            if(int_to_string_ip(ip , ip_str[i]) < 0)
            {
                err = true;
                break;
            }
            if(DBP->bind_string_parameter(pre , index , ip_str[i]) < 0)
            {
                err = true;
                break;
            }
        }
        //对于不绑定的参数，对于字符串系统输入的数据时"0",但是这里自己定义
        else
        {
            if(DBP->bind_string_parameter(pre , index , INVALID_IP_STR) < 0)
            {
                err = true;
                break;
            }
        }
    }

OUT:
    if(err)
    {
        LOG_ERROR("CSDBControl::bind parameter error !");
        return -1;
    }
    if(DBP->prepare_execute_operation(pre) < 0)
    {
        LOG_ERROR("CSDBControl::prepare execute an operation error !");
        return -1;
    }
    
    return 0;
}


int InitSystemWorkItem::log_init_info(Connection_T conn , RULE_TYPE type , int dup , int mod)
{
    OP_TYPE op_t;
    if(MU_RULER == type)
        op_t = MU_INIT_DEPLOY;
    else 
        op_t = SU_INIT_DEPLOY;

//    std::string info_pro = init_info_string();
    std::string info_pro = "init deploy success !";
    std::cerr<<info_pro<<std::endl;
    int id = DBP->log_op_info_start(conn , op_t , info_pro);
    if(id < 0)
    {
        LOG_ERROR("CSDBControl::Log init info start error !");
        return -1;
    }
    std::string addition = int_to_str(dup) + ":" + int_to_str(mod);  //暂时使用这种方式存储初始化模值和副本数
    return DBP->log_op_info_end(conn , id , true , addition);
}

int InitSystemWorkItem::init_cstore_database(RULE_TYPE type , const std::vector<struct HashRule> &vec , int dup_num , int mod)
{
    int items = vec.size();
    std::string sql;
    PreparedStatement_T pre = NULL;
    if(0 == items)
    {
        LOG_ERROR("CSDBControl::Empty Init infomations before create database!");
        return 0;
    }

    Connection_T conn_p = NULL;
    conn_p = DBP->create_connection();
    if(NULL == conn_p)
    {
        LOG_ERROR("CSDBControl::Create connection error when init catore database!");
        return -1;
    }

    if(init_tables(conn_p , type) < 0)
    {
        LOG_ERROR("CSDBControl::Init create and drop tables error !");
        goto ERR;
    }

    sql = insert_rule_prefix + DBP->get_rule_table_name(type) + insert_rule_prepare;
    if((pre = DBP->prepare_execute(conn_p , sql)) == NULL)
    {
        LOG_ERROR("CSDBControl::Prepare statement execute sql error : " + sql);
        goto INSERT_ERR ;
    }
    for(int i = 0 ; i < items ; ++ i)
    {
        if(generate_every_bucket_rule(pre , vec[i]) < 0)
        {
            LOG_ERROR("CSDBControl::generate one bucket rule to database error !");
            goto INSERT_ERR ;
        }
    }

    if(log_init_info(conn_p , type , dup_num , mod) < 0)
    {
        LOG_ERROR("CSDBControl::execute insert duplicate and mod info error !");
    }

    DBP->close_connection(conn_p);
    conn_p = NULL;
    return 0;

INSERT_ERR : 
    //为了保证操作的完整性，这里应该清楚所有以创建的表，暂时不实现
ERR :
    DBP->close_connection(conn_p);
    conn_p = NULL;
    return -1;
}

std::string InitSystemWorkItem::init_info_string()
{
    std::string rule;
    struct HashRule hash;
    std::string one;
    uint32_t ip = 0;
    int bucket_num = m_init_info.rule_table.size();

    for(int i = 0 ; i < bucket_num ; ++ i)
    {
        hash = (m_init_info.rule_table)[i];
        one = int_to_str(hash.bucket_nr) + ":";
        int j = 0;
        for(j = 0 ; j < MAX_IP_NUM ; ++ j)
        {
            ip = (hash.ip)[j];
            if(INVALID_IP == ip)
                break;
        }
        for(int k = 0 ; k < j ; ++ k)
        {
            one += int_to_str((hash.ip)[k]);
            if(k != j - 1)
                one += ":";
        }

        rule += one;
        if(i != bucket_num - 1)
            rule += "|";
    }

    return rule;
}

//首先需要创建表!!!
int InitSystemWorkItem::init_operation_table(Connection_T conn)
{
    std::string check_sql = std::string("show tables like ") + "\"" 
        +CSConfiguration::getInstance()->history_table + "\"";

    ResultSet_T ret = DBP->execute_query(conn , check_sql);
    if(NULL == ret)
    {
        LOG_ERROR("InitSystemWorkItem::execute query table error : " + check_sql);
        return -1;
    }
    //表不存在
    if(!ResultSet_next(ret))
    {
        std::string create_sql = std::string(create_prefix) 
            + CSConfiguration::getInstance()->history_table + history_table_attr;
        if(DBP->execute_operation(conn , create_sql) < 0)
        {
            LOG_ERROR("InitSystemWorkItem::execute create history table error : " + create_sql);
            return -1;
        }
        return 0;
    }

    std::vector<OP_TYPE> other_type;
    //这里只能手工添加了。唉...
    if(MU_RULER == m_rule_type)
    {
        other_type.push_back(SU_MIGRATION);
        other_type.push_back(SU_EXTENT);
        other_type.push_back(SU_INIT_DEPLOY);
    }
    else
    {
        other_type.push_back(MU_MIGRATION);
        other_type.push_back(MU_EXTENT);
        other_type.push_back(MU_INIT_DEPLOY);
    }
    other_type.push_back(RUBBISH_RECYCLER);

    int size = other_type.size();
    std::string del_sql = "delete from " + CSConfiguration::getInstance()->history_table
        + " where (";
    
    for(int i = 0 ; i < size ; ++ i)
    {
        del_sql += std::string("type != \"") + op_types[other_type[i]] + "\"";
        if(i != size - 1)
           del_sql += " and ";
    }
    del_sql += ")";

    std::cerr<<"delete sql : "<<del_sql<<std::endl;

    if(DBP->execute_operation(conn , del_sql) < 0)
    {
        LOG_ERROR("InitSystemWorkItem::execurate delete sql error : " + del_sql);
        return -1;
    }

    return 0;
}
