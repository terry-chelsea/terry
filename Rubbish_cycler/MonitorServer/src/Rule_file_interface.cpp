/*
 * =====================================================================================
 *
 *       Filename:  Rule_file_interface.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/12 09:50:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Rule_file_interface.h"
#include "Lib_tool.h"

int FileOperation::init_CS_all_rule()
{
    std::vector<struct HashRule> all_hash_rule;
    std::vector<struct OrderRule> all_order_rule;

    if(read_all_hash_rule_from_file(MU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all MU hash rule from file error !");
        return -1;
    }
    if(appaly_hash_rule_to_manager(MU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::MU RULE is ERROR , Please Check!");
        return -1;
    }

    if(read_all_hash_rule_from_file(SU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all SU hash rule from file error !");
        return -1;
    }
    if(appaly_hash_rule_to_manager(SU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::SU RULE is ERROR , Please Check!");
        return -1;
    }

    if(read_all_order_rule_from_file(MU_RULER , all_order_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all MU order rule from file error !");
        return -1;
    }
    appaly_order_rule_to_manager(MU_RULER , all_order_rule);

    if(read_all_order_rule_from_file(SU_RULER , all_order_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all SU order rule from file error !");
        return -1;
    }
    appaly_order_rule_to_manager(SU_RULER , all_order_rule);

    return 0;
}

void FileOperation::display_all_rule_for_debug()
{
    std::vector<struct HashRule> all_hash_rule;
    std::vector<struct OrderRule> all_order_rule;
    if(read_all_hash_rule_from_file(MU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all MU hash rule from file error !");
        return ;
    }
    
    display_rule(all_hash_rule , MU_RULER);

    if(read_all_hash_rule_from_file(SU_RULER , all_hash_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all SU hash rule from file error !");
        return ;
    }
    display_rule(all_hash_rule , SU_RULER);

    if(read_all_order_rule_from_file(MU_RULER , all_order_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all MU order rule from file error !");
        return ;
    }
    display_order(all_order_rule , MU_RULER);

    if(read_all_order_rule_from_file(SU_RULER , all_order_rule) < 0)
    {
        LOG_ERROR("FileOperation::read all SU order rule from file error !");
        return ;
    }
    display_order(all_order_rule , SU_RULER);
}

void FileOperation::display_rule(std::vector<struct HashRule> &all_hash_rule , RULE_TYPE type)
{
    std::string type_str = (MU_RULER == type ? "MU" : "SU");
    std::cout<<"------------------------"<<type_str<<" RULE INFO----------------------"<<std::endl;
    std::vector<struct HashRule>::iterator it = all_hash_rule.begin();
    std::vector<struct HashRule>::iterator it_end = all_hash_rule.end();
    struct HashRule rule_temp;
    for( ; it != it_end ; ++ it)
    {
        rule_temp = *it;
        std::cout<<rule_temp.bucket_nr;
        for(int i = 0 ; i < MAX_IP_NUM ; ++ i)
        {
            if(INVALID_IP == rule_temp.ip[i])
                continue ;
            std::cout<<"  "<<string_ip(rule_temp.ip[i]);
        }

        std::cout<<std::endl;
    }
}

void FileOperation::display_order(std::vector<struct OrderRule> &all_order_rule , RULE_TYPE type)
{
    std::string type_str = (MU_RULER == type ? "MU" : "SU");
    if(all_order_rule.empty())
    {
        std::cout<<type_str<<" order is empty !"<<std::endl;
        return ;
    }

    std::cout<<"------------------------"<<type_str<<" ORDER INFO----------------------"<<std::endl;
    std::vector<struct OrderRule>::iterator it = all_order_rule.begin();
    std::vector<struct OrderRule>::iterator it_end = all_order_rule.end();
    struct OrderRule order_temp;

    for( ; it != it_end ; ++ it)
    {
        order_temp = *it;
        std::cout<<(order_temp.cmd == MIGRATION ? "migration" : "extention")<<"  "<<order_temp.version;
        if(order_temp.cmd == MIGRATION)
        {
            std::cout<<"  "<<order_temp.order.migration.bucket_nr<<"  "<<string_ip(order_temp.order.migration.src_ip)<<"  "<<string_ip(order_temp.order.migration.dest_ip)<<std::endl;
        }
        else
        {
            std::cout<<"  "<<order_temp.order.extent.new_mod<<std::endl;
        }
    }
}

std::string FileOperation::string_ip(uint32_t ip)
{
    std::string ret;
    if(int_to_string_ip(ip , ret) < 0)
    {
        std::cerr<<"int to string ip error !"<<std::endl;
        ret = "error !";
    }

    return ret;
}

int FileOperation::read_all_hash_rule_from_file(RULE_TYPE type , std::vector<struct HashRule> &rule_table)
{
    rule_table.clear();
    std::string file_name = generate_file_name(type , true);

    struct stat stat_buf;
    memset(&stat_buf , 0 , sizeof(stat_buf));
    int file_size = 0;
    uint32_t items = 0;
    struct HashRule *rule_buf = NULL;

    int fd = open(file_name.c_str() , O_RDONLY);
    if(fd < 0)
    {
        if(ENOENT == errno) 
        {
            LOG_EVENT("*********Init System Hash Rule First !**********");
            goto RET;
        }
        else
        {
            LOG_SYSCALL_ERROR("FileOperation::opne file " + file_name + " error !");
            return -1;
        }
    }

    if(fstat(fd , &stat_buf) < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Stat file " + file_name + " error !");
        goto ERR ;
    }

    file_size = stat_buf.st_size;
    if(0 == file_size)
    {
        LOG_EVENT("-----------Hash Rule file " + file_name + " is Empty !-------------");
        goto ERR ;
    }
    //如果文件大小不是整数个HashRule个对象大小的话，说明规则表错误
    if(file_size % HASH_STRUCT_SIZE)
    {
        LOG_ERROR("FileOperation::Hash rule file " + file_name + " Hash been Destoryed !");
        goto ERR ;
    }

    items = file_size / HASH_STRUCT_SIZE;
    rule_buf = new struct HashRule[items];
    memset(rule_buf , 0 , file_size);

    if(read(fd , rule_buf , file_size) != file_size)
    {
        LOG_SYSCALL_ERROR("FileOperation::Read hash rule from file " + file_name + " error !");
        goto FREE ;
    }
    rule_table.reserve(items);

    for(uint32_t i = 0 ; i < items ; ++ i)
    {
        if(rule_buf[i].bucket_nr != i)
        {
            LOG_ERROR("FileOperation::Hash rule file " + file_name + " Is ERROR !" 
                    + "items " + int_to_str(i) + " Correspondence to bucket " 
                    + int_to_str(rule_buf[i].bucket_nr));
            goto FREE ;
        }

        rule_table.push_back(rule_buf[i]);
    }

RET :
    close(fd);
    return 0;

FREE :
    delete [] rule_buf;
    rule_buf = NULL;

ERR :
    close(fd);
    return -1;
}

int FileOperation::read_all_order_rule_from_file(RULE_TYPE type , std::vector<struct OrderRule> &rule_table)
{
    rule_table.clear();
    std::string file_name = generate_file_name(type , false);

    struct stat stat_buf;
    memset(&stat_buf , 0 , sizeof(stat_buf));
    int file_size = 0;
    uint32_t items = 0;
    struct OrderRule *rule_buf = NULL;

    int fd = open(file_name.c_str() , O_RDONLY);
    if(fd < 0)
    {
        if(ENOENT == errno || EEXIST == errno)
        {
            LOG_EVENT("*********Init System Hash Rule First !**********");
            goto RET;
        }
        else
        {
            LOG_SYSCALL_ERROR("FileOperation::opne file " + file_name + " error !");
            return -1;
        }
    }

    if(fstat(fd , &stat_buf) < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Stat file " + file_name + " error !");
        goto ERR ;
    }

    file_size = stat_buf.st_size;
    if(0 == file_size)
    {
//        LOG_EVENT("-----------Order Rule file " + file_name + " is Empty !-------------");
        goto RET ;
    }
    //如果文件大小不是整数个HashRule个对象大小的话，说明规则表错误
    if(file_size % ORDER_STRUCT_SIZE)
    {
        LOG_ERROR("FileOperation::Order rule file " + file_name + " Hash been Destoryed !");
        goto ERR ;
    }

    items = file_size / ORDER_STRUCT_SIZE;
    rule_buf = new struct OrderRule[items];
    memset(rule_buf , 0 , file_size);
    rule_table.reserve(items);

    if(read(fd , rule_buf , file_size) != file_size)
    {
        LOG_SYSCALL_ERROR("FileOperation::Read Order rule from file " + file_name + " error !");
        goto FREE ;
    }


    //第0项不用读取
    for(uint32_t i = 1 ; i < items ; ++ i)
    {
        if(rule_buf[i].version != i)
        {
            LOG_ERROR("FileOperation::Hash rule file " + file_name + " Is ERROR !" 
                    + "items " + int_to_str(i) + " Correspondence to version " 
                    + int_to_str(rule_buf[i].version));
            goto FREE ;
        }

        rule_table.push_back(rule_buf[i]);
    }

RET :
    close(fd);
    return 0;

FREE :
    delete [] rule_buf;
    rule_buf = NULL;

ERR :
    close(fd);
    return -1;
}

int FileOperation::appaly_hash_rule_to_manager(RULE_TYPE type , const std::vector<struct HashRule> &rule_table)
{
    int vec_size = rule_table.size();
    for(int i = 0 ; i < vec_size ; ++ i)
    {
        RulerManager::getInstance()->add_new_hash_item(rule_table[i] , type);
    }

    int mod = cs_log(vec_size);
    if(mod < 0)
    {
        LOG_ERROR("FileOperation::Rule is error !");
        return -1;
    }
    RulerManager::getInstance()->set_current_mod(type , mod);
    return 0;
}

void FileOperation::appaly_order_rule_to_manager(RULE_TYPE type , const std::vector<struct OrderRule> & rule_table)
{
    int vec_size = rule_table.size();
    for(int i = 0 ; i < vec_size ; ++ i)
    {
        RulerManager::getInstance()->add_new_order_item(rule_table[i] , type);
    }

    RulerManager::getInstance()->set_current_version(type , vec_size);
    std::cerr<<"init current verson : "<<RulerManager::getInstance()->get_current_version(type)<<std::endl;
}

int FileOperation::generate_CS_init_rule(RULE_TYPE type , const std::vector<struct HashRule> &rule_table)
{
    const struct HashRule *rule_buf = &rule_table[0];
    int items = rule_table.size();
    if(0 == items)
    {
        LOG_EVENT("Empty Hash rule table !");
        return 0;
    }

    if(mkdir(FILE_DIR , DIR_MODE) < 0)
    {
        if(errno != EEXIST)
        {
            LOG_SYSCALL_ERROR("FileOperation::Create directory " + std::string(FILE_DIR) + " error !");
            return -1;
        }
    }

    std::string file_name = generate_file_name(type , true);
    std::string order_file_name = generate_file_name(type , false);

    int fd = open(file_name.c_str() , O_WRONLY | O_CREAT | O_TRUNC , FILE_MODE);
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Create file " + file_name + " error !");
        return -1;
    }
    
    if(write(fd , rule_buf , items * HASH_STRUCT_SIZE) != items * HASH_STRUCT_SIZE)
    {
        LOG_SYSCALL_ERROR("FileOperation::Write hash rule to file " + file_name + "errro !");
        close(fd);
        return -1;
    }

    close(fd);

    fd = open(order_file_name.c_str() , O_WRONLY | O_CREAT | O_TRUNC , FILE_MODE);
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Create file " + order_file_name + " error !");
        return -1;
    }
    if(write(fd , &EMPTY_VERSION , sizeof(EMPTY_VERSION)) != sizeof(EMPTY_VERSION))
    {
        LOG_SYSCALL_ERROR("FileOperation::write init order rule to file " + order_file_name + " error !");
        close(fd);
        return -1;
    }
    close(fd);

    appaly_hash_rule_to_manager(type , rule_table);

    return 0;
}

int FileOperation::modify_file_case_migration(RULE_TYPE type, struct Migration_Info info)
{
    struct OrderRule order_rule;
    memset(&order_rule , 0 , sizeof(order_rule));

    //保证首先修改文件，然后再去修改内存的内容
    order_rule.version = RulerManager::getInstance()->get_current_version(type) + 1;
    order_rule.cmd = MIGRATION;
    order_rule.order.migration = info;

    int bucket_num = info.bucket_nr;
    uint32_t src_ip = info.src_ip;
    struct HashRule hash_rule;
    memset(&hash_rule , 0 , sizeof(hash_rule));
    RulerManager::getInstance()->get_hash_rule(bucket_num , type , hash_rule);
    int i = 0;
    for( ; i < MAX_IP_NUM ; ++ i)
    {
        if(src_ip == hash_rule.ip[i])
            break;
    }

    if(MAX_IP_NUM == i)
    {
        LOG_ERROR("FileOperation::Can not find source IP from bucket " 
                + int_to_str(bucket_num) + " rule !");
        return -1;
    }

    //修改对应的IP地址
    hash_rule.ip[i] = info.dest_ip;

    off_t offset = bucket_num * HASH_STRUCT_SIZE;
    std::string file_name = generate_file_name(type , true);
    int fd = open(file_name.c_str() , O_WRONLY);
    
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Open hash rule file " + file_name + " error !");
        return -1;
    }

    //首先将全部的数据读入到内存，如果第二次修改失败，恢复之前的状态
    std::string file_content;
    uint32_t file_size = 0;
    if(generate_content(type , true , file_content) < 0)
    {
        LOG_ERROR("FileOperation::generate all hash file content error !");
        close(fd);
        return -1;
    }
    file_size = file_content.size();
    
    if(pwrite(fd , &hash_rule , HASH_STRUCT_SIZE , offset) != HASH_STRUCT_SIZE)
    {
        LOG_SYSCALL_ERROR("FileOperation::Pwrite data to file " + file_name + " error !");
        close(fd);
        return -1;
    }
    
    if(append_order_rule(type , order_rule) < 0)
    {
        LOG_ERROR("FileOperation::Append migration info error !");
        if(pwrite(fd , file_content.c_str() , file_size , 0) != file_size)
        {
            LOG_SYSCALL_ERROR("FileOperation::Pwrite data to file " + file_name + " error !");
        }
        close(fd);
        return -1;
    }

    close(fd);
    
    std::cerr<<"modify file of migration successfully !"<<std::endl;

    return 0;
}

std::string FileOperation::generate_file_name(RULE_TYPE type , bool choise)
{
    std::string file_name;
    file_name = FILE_DIR;

    if(type == MU_RULER)
        file_name += MU_PREFIX;
    else if(type == SU_RULER)
        file_name += SU_PREFIX;
    else
        file_name += "/123/456";
    
    if(choise)
        file_name += HASH_FILE_NAME;
    else
        file_name += ORDER_FILE_NAME;

    return file_name;
}

int FileOperation::append_order_rule(RULE_TYPE type , struct OrderRule order_rule)
{
    std::string file_name = generate_file_name(type , false);
    int fd = open(file_name.c_str() , O_WRONLY | O_APPEND);
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::append type open file " + file_name + "error !");
        return -1;
    }

    if(write(fd , &order_rule , ORDER_STRUCT_SIZE) != ORDER_STRUCT_SIZE)
    {
        LOG_SYSCALL_ERROR("FileOperation::Write data to file " + file_name + " error !");
        close(fd);
        return -1;
    }
    close(fd);
    
    return 0;
}

int FileOperation::modify_file_case_extent(RULE_TYPE type , struct Extent_Info info)
{
    uint32_t mod = info.new_mod;
    uint32_t current_mod = RulerManager::getInstance()->get_current_mod(type);
    //先修改文件，然后在修改内存
    if(mod != current_mod + 1)
    {
        LOG_ERROR("FileOperation::current system mod is " 
                + int_to_str(current_mod) + " modify mod is " + int_to_str(mod));
        return -1;
    }

    std::vector<struct HashRule> rule_table;
    struct HashRule hash_rule;
    memset(&hash_rule , 0 , HASH_STRUCT_SIZE);
    uint32_t bucket_num = RulerManager::getInstance()->get_bucket_number(type);

    for(uint32_t i = 0 ; i < bucket_num ; ++ i)
    {
        RulerManager::getInstance()->get_hash_rule(i , type , hash_rule);
        hash_rule.bucket_nr += bucket_num;
        rule_table.push_back(hash_rule);
    }
    
    std::string file_name = generate_file_name(type , true);
    int fd = open(file_name.c_str() , O_WRONLY | O_APPEND);
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::append type open hash file " + file_name + "error !");
        return -1;
    }

    //首先将全部的数据读入到内存，如果第二次修改失败，恢复之前的状态
    std::string file_content;
    uint32_t file_size;
    if(generate_content(type , true , file_content) < 0)
    {
        LOG_ERROR("FileOperation::generate all hash file content error !");
        close(fd);
        return -1;
    }
    file_size = file_content.size();

    uint32_t write_size = bucket_num * HASH_STRUCT_SIZE;
    if(write(fd , &rule_table[0] , write_size) != write_size)
    {
        LOG_SYSCALL_ERROR("FileOperation::write data to hash file " + file_name + " error !");
        close(fd);
        return -1;
    }

    struct OrderRule order_rule;
    order_rule.cmd = EXTENT;
    order_rule.version = RulerManager::getInstance()->get_current_version(type) + 1;
    order_rule.order.extent = info;

    if(append_order_rule(type , order_rule) < 0)
    {
        LOG_ERROR("FileOperation::Append extent info error !");
        if(pwrite(fd , file_content.c_str() , file_size , 0) != file_size)
        {
            LOG_SYSCALL_ERROR("FileOperation::Pwrite data to file " + file_name + " error !");
        }
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

int FileOperation::generate_all_content(RULE_TYPE type , std::string &hash_content , std::string &order_content)
{
    if((generate_content(type , true , hash_content) < 0) ||
     (generate_content(type , false , order_content) < 0) )
    {
        LOG_ERROR("FileOperation::generate all file content error !");
        return -1;
    }

    return 0;
}

int FileOperation::generate_content(RULE_TYPE type , bool choise , std::string &content)
{
    std::string file_name = generate_file_name(type , choise);
    int fd = open(file_name.c_str() , O_RDONLY);
    if(fd < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Open file " + file_name + " error !");
        return -1;
    }

    struct stat stat_buf;
    memset(&stat_buf , 0 , sizeof(stat_buf));
    if(fstat(fd , &stat_buf) < 0)
    {
        LOG_SYSCALL_ERROR("FileOperation::Fstat file " + file_name + " errror !");
        close(fd);
        return -1;
    }

    int file_size = stat_buf.st_size;
    char *file_content = new char[file_size];
    memset(file_content , 0 , file_size);

    if(read(fd , file_content , file_size) != file_size)
    {
        LOG_SYSCALL_ERROR("FileOperation::Read file " + file_name + " error !");
        close(fd);
        delete file_content;
        file_content = NULL;
        return -1;
    }

    content.assign(file_content , file_size);

    delete file_content;
    file_content = NULL;
    close(fd);

    return 0;
}
