/*
 * =====================================================================================
 *
 *       Filename:  Rule_file_interface.h
 *
 *    Description:  为上层访问文件中的规则信息提供一些接口
 *
 *        Version:  1.0
 *        Created:  11/29/12 01:47:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com), terry
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#ifndef _H_HASH_FILE_INTERFACE_H_
#define _H_HASH_FILE_INTERFACE_H_

//当前的CS版本将规则存放在本地的一个目录下的四个文件中
//分别为MU和SU的增量规则和桶规则保存一个文件，其中存放的是struct OrderRule
//和struct HashRule对象的结构化数据，之所以使用文件是因为文件更容易实现，
//当然是相对于数据库，但是数据库优于文件的是结构化数据的查找和存放的速度
//在当前CS中所有的规则信息全部保存在内存中，这几个文件只用于持久性存储
//因此不存在大量的查找文件的操作，可能引起文件的操作有以下几个方面：
//1、初始化CS的时候需要将文件中的所有信息读取到内存，这时候的文件可以是本地
//的，也可以是在本地丢失的情况下从MPC那里拉取回来的规则。本次读取将会读取全部的四个文件的数据，并对其进行解释，存放在内存中。
//2、MPC初始化部署系统的时候，需要将计算之后的初始化规则固化到文件中，这次需要创建这四个文件，对桶规则文件写入全部的桶规则，对增量规则只写入规则为0的信息，当然，它是无效的一个规则版本。
//3、当系统桶迁移的时候需要修改系统的规则信息，需要在增量规则表中append一项，并且修改当前的桶规则表的内容，这时候采用特定偏移量的方式操作，需要对文件进行判断。
//4、当系统进行桶扩展的时候，需要修改增量规则表的内容，以append的方式写入一个新的规则表项，也需要修改桶规则表，同样是以append的方式添加
//5、为了编译灾难恢复（CS硬盘损坏），需要将这些规则文件定期的交给MPC保管，相当于做一个备份，具体的做法是在规则表更新的时候将文件的内容全部发送给MPC，例如执行一次桶迁移操作，在桶迁移操作结束之前设置一个定时器，默认为3秒，执行发送工作，如果失败则会一直重试直到成功。每次只发送修改的文件
//
//根据系统的定义struct HashRule对象的大小事24字节，struct OrderRule对象的大小是20字节，假设系统极限时候存在4096个桶，规则表的信息也只有96K，增量规则的极限是10*1024，增量规则表的大小只有240K,因此每次发送全部的文件将不会有任何问题，这中方式增加了MPC的两个功能，接受文件和提供文件。
//

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <vector>

#include "Lib_tool.h"
#include "Log_warp.h"
#include "RulerManager.h"

#define FILE_DIR "/root/.cs_rule/"
#define MU_PREFIX ".MU_"
#define SU_PREFIX ".SU_"
#define HASH_FILE_NAME "hash.t"
#define ORDER_FILE_NAME "order.t"

#define FILE_MODE (S_IRUSR | S_IWUSR)
#define DIR_MODE  (S_IRUSR | S_IWUSR | S_IXUSR)

uint32_t const HASH_STRUCT_SIZE = sizeof(struct HashRule);
uint32_t const ORDER_STRUCT_SIZE = sizeof(struct OrderRule);

class FileOperation
{
    //不允许创建对象
    private :
        FileOperation()
        {}
        ~FileOperation()
        {}

    public : 
        //CS初始化，初始化全部的规则
        static int init_CS_all_rule();

        //MPC部署系统的时候初始化全部的CS规则，桶规则，增量规则只填充第一项
        static int generate_CS_init_rule(RULE_TYPE , const std::vector<struct HashRule> &);

        //由于桶迁移而修改规则文件，包括桶规则文件和增量规则文件
        static int modify_file_case_migration(RULE_TYPE , struct Migration_Info);

        //由于桶扩展操作而修改规则文件
        static int modify_file_case_extent(RULE_TYPE , struct Extent_Info);

        static int generate_all_content(RULE_TYPE , std::string & , std::string &);


        static void display_all_rule_for_debug();

        static std::string string_ip(uint32_t ip);
    private :
        static int read_all_hash_rule_from_file(RULE_TYPE , std::vector<struct HashRule> &);

        static int read_all_order_rule_from_file(RULE_TYPE , std::vector<struct OrderRule> &);

        static int appaly_hash_rule_to_manager(RULE_TYPE , const std::vector<struct HashRule> &);

        static void appaly_order_rule_to_manager(RULE_TYPE , const std::vector<struct OrderRule> &);

        static int generate_content(RULE_TYPE , bool , std::string &);
    
        static std::string generate_file_name(RULE_TYPE , bool);

        static int append_order_rule(RULE_TYPE , struct OrderRule);




        static void display_order(std::vector<struct OrderRule> &all_order_rule , RULE_TYPE type);

        static void display_rule(std::vector<struct HashRule> &all_hash_rule , RULE_TYPE type);
};

#endif
