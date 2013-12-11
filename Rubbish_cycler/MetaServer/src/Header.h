/*
 * =====================================================================================
 *
 *       Filename:  header.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/05/12 19:14:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */
#ifndef _H_TERRY_HEADER_H_
#define _H_TERRY_HEADER_H_

//这里包含一些辅助性的文件
#include "List.h"
#include "Log/Debug.h"
#include "XMLConfig.h"
#include "Timer.h"
#include "Tool.h"
#include "Memory.h"
#include "Traverse.h"
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "../protocol/recycle_protocol.h"


#define STR_ERROR  strerror(errno) 

#define FILE_BLOCK_SIZE (512 * 1024)

#define BLOCK_PREFIX  8                                //默认的分块的前缀，使用infohash的前多少个字节进行分块，默认为8
#define BLOCK_NUMBER  (1 << BLOCK_PREFIX)              //默认的分块数

#define START_CHAR    -1         //管道1传输的是int类型,启动垃圾回收
#define FINISH_ALL    -1         //管道2传输的也是int类型,垃圾回收全部完成
#define STOP_CHAR     -2         //管道1通知子线程暂停,子线程需要暂停
#define CONT_CHAR     'R'        //等到该字符才继续,通过管道3发送

#define CS_TIMEOUT    (2 * 60)                         //判断CS超时的最大时限，如果这段时间内没有心跳，进行探测.
#define CHECK_RECOVER_INTERVAL 10                      //查看处于恢复状态桶的间隔值

/* In header.para1 */
/* MU to CS */
#define HEARTBEAT_ONE_BUCKET_FINISH             100             //MU向CS，一个桶完成了，需要负载数据
#define HEARTBEAT_ALL_BUCKET_FINISH             101             //所有的桶都完成了，一般不需要负载数据
#define HEARTBEAT_HEARTBEAT                     102             //普通的心跳信息

/* CS to MU */
#define HEARTBEAT_BEGIN_RECYCLE                 200             //启动一次垃圾回收，负载为本次垃圾回收的参数，和所有桶号
#define HEARTBEAT_ADD_BUCKET                    201             //向MU追加一个需要遍历的新桶
#define HEARTBEAT_HEARTBEAT_ACK                 202             //普通的心跳回复
//暂时还没考虑CS暂停垃圾回收工作。

typedef struct 
{
    char *g_all_memory;                      //所有位图占用的内存，默认为2G空间，但不一定全部使用
    unsigned long block_bitmap_size;         //每个分块位图大小，根据CS命令确定
    int32_t block_num;                       //分块数，默认是256，如果CS命令显示定义，根据CS命令确定
    int32_t hash_func_num;                   //哈希函数说明，根据CS命令确定
    int main_to_thread[2];                   //第一根管道，主线程向子线程发送信息、
    int thread_to_main[2];                   //第二根管道，子线程向主线程发送信息
    int wakeup_and_block[2];                 //第三根管道，用于子线程的阻塞与启动

    //这个链表是两个线程都需要访问的，但是有严格的先后顺序，所以不需要加锁
    NODE *normal_bucket_list;                //启动垃圾回收的时候将所有不处于恢复状态的桶添加到这个链表，以后不再改变。
    //这三个链表只能由主线程访问
    NODE *recovery_bucket_list;              //每次判断之后将处于恢复状态的桶加入到这个链表
    NODE *append_bucket_list;                //将所有从恢复状态转化成工作状态的桶加入到这个链表，发送给子线程
    NODE *finish_bucket_list;                //将所有子线程完成的桶加入到这个链表上，发送给CS

    int g_listen_fd;                         //监听套接字
    int check_disk_fd;                       //查看磁盘的套接字
    int send_heartbeat_fd;                   //向CS发送心跳的定时器套接字
    int heartbeat_ack_fd;                    //与CS保持心跳连接的套接字

    int basic_timer_counter;                 //基本定时器超时次数，用于检查磁盘和回复队列
    int thread_blocking;                     //子线程是否正在阻塞。

    int I_finish;                            //子线程是否已经完成了一次垃圾回收的全部遍历。
    int in_connecting;                       //当前是否正处于垃圾回收状态，
}GLOBLE_VALUE;

extern GLOBLE_VALUE g_global_value;



//下面的定义是系统中负载数据的类型结构，由于protobuf不支持C语言，所以只能定义二进制了。呼呼。。。
//由于需要使用网络传递，所以必须使不依赖于机器的数据类型，在stdint中定义。
//MPC --- CS
typedef struct
{
    uint64_t scale;                              //垃圾回收的规模
}MPC_CS_RUBBISH_RECYCLE_struct;

//CS --- MU
//注意，由于这里需要使用不定数组，所以这个不定数组实际存放在这个结构体的后面，由array_size标志这个数组的大小，数组元素的类型是int类型
typedef struct 
{
    uint64_t bitmap_size;                      //bloom filter 计算的位图大小
    int32_t  hash_func_number;                 //bloom filter 计算的哈希函数个数
    int32_t  block_prefix;                     //CS决定的分块大小，一般为8
    int32_t  array_size;                       //接下来负载的大小，是一个数组，数组类型是int32_t，元素个数保存在此，数组紧跟着这个结构体
}CS_MU_RUBBISH_RECYCLE_struct;

typedef struct 
{
    int32_t  bucket_nr;                        //追加的桶号
}CS_MU_ADD_ONE_BUCKET_struct;

//CS --- RU
typedef struct
{
    int32_t hash_func_number;                 //CS计算的哈希函数的个数
    int32_t array_size;                       //同前面的array_size 
}CS_RU_RUBBISH_RECYCLE_struct;

//MU --- CS
typedef struct 
{
    int32_t bucket_nr;                       //完成的桶号
}MU_CS_FINISH_ONE_BUCKET_struct;

typedef struct 
{
    int32_t array_size;                      //所有完成的桶号
}MU_CS_FINISH_ALL_BUCKET_struct;

//RU --- RU
typedef struct 
{
    int string_size;                        //一个infohash值
}SU_SU_HANDLE_RESULE_struct;


#endif
