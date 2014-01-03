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
#include "HashFunc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>


#include "../protocol/recycle_protocol.h"

typedef enum 
{
    WATING = 0 ,                            //无连接时...
    CONNECTING ,                            //接收到连接之后，尚未进行任务分配...
    WORKING ,                               //执行位图生成过程..
    UPLOAD ,                                //完成位图的生成...
    STOPING ,
}State;

typedef struct 
{
    char *g_all_memory;                      //所有位图占用的内存，默认为2G空间，但不一定全部使用
    unsigned long block_bitmap_size;         //每个分块位图大小，根据CS命令确定
    int32_t block_num;                       //分块数，默认是256，如果CS命令显示定义，根据CS命令确定
    HASH_FUNC selected_hash_func[MAX_HASH_NUMBER];     //当前选中的哈希函数...
    int32_t hash_func_num;                   //哈希函数说明，根据CS命令确定
    int main_to_thread[2];                   //第一根管道，主线程向子线程发送信息、
    int thread_to_main[2];                   //第二根管道，子线程向主线程发送信息
    int wakeup_and_block[2];                 //第三根管道，主线程向子线程发起检测系统的命令
    State current_state;                     //系统的状态机
    uint32_t current_bucket;                 //当前回收的桶号

    int g_listen_fd;                         //监听套接字
    int send_heartbeat_fd;                   //向CS发送心跳的定时器套接字
    int connectting_fd;                      //与CS保持心跳连接的套接字
    int in_connecting;                       //当前是否正处于垃圾回收状态，
}GLOBLE_VALUE;

extern GLOBLE_VALUE g_global_value;

//0x5243 = RC in ASCII...
const uint32_t RC_CS_NODE_INIT_MSG = 0x52430000;
const uint32_t RC_CS_NODE_INIT_MSG_ACK = 0x52430001;
typedef struct 
{
    uint64_t bitmap_size;
    uint16_t block_bits;
    uint16_t hash_funcs_num;
    uint8_t  compress_flag;
    uint8_t  token_flag;
    uint16_t heartbeat_interval;
    char     nfs_path[32];
    char     hash[20];
}NodeInitInfo;

const uint32_t RC_CS_NODE_HEARTBEAT_MSG = 0x52430002;
const uint32_t RC_CS_NODE_HEARTBEAT_MSG_ACK = 0x52430003;
typedef struct 
{
    uint32_t bucket_nr;
    uint32_t state;
    uint32_t finish_items;
    uint32_t cpu_rate;
    uint32_t disk_state;
    uint32_t net_speed;
}HeartbeartInfo;

const uint32_t RC_CS_NODE_STOP_RECYCLER_MSG = 0x52430004;
const uint32_t RC_CS_NODE_STOP_RECYCLER_MSG_ACK = 0x52430005;

const uint32_t RC_CS_NODE_CONTINUE_RECYCLER_MSG = 0x52430006;
const uint32_t RC_CS_NODE_CONTINUE_RECYCLER_MSG_ACK = 0x52430007;

const uint32_t RC_CS_NODE_CANCEL_RECYCLER_MSG = 0x52430008;
const uint32_t RC_CS_NODE_CANCEL_RECYCLER_MSG_ACK = 0x52430009;

const uint32_t RC_CS_NODE_START_BUCKET_MSG = 0x5243000A;
const uint32_t RC_CS_NODE_START_BUCKET_MSG_ACK = 0x5243000B;

const uint32_t RC_CS_NODE_CANCEL_BUCKET_MSG = 0x5243000C;
const uint32_t RC_CS_NODE_CANCEL_BUCKET_MSG_ACK = 0x5243000D;
typedef struct 
{
    uint32_t bucket_nr;
}BucketInfo;

const uint32_t RC_NODE_CS_GET_TOKEN_MSG = 0x5243000E;
const uint32_t RC_NODE_CS_GET_TOKEN_MSG_ACK = 0x5243000F;

const uint32_t RC_NODE_CS_PUT_TOKEN_MSG = 0x52430010;
const uint32_t RC_NODE_CS_PUT_TOKEN_MSG_ACK = 0x52430011;

const uint32_t RC_MU_CS_UPLOAD_BITMAP_MSG = 0x52430012;
const uint32_t RC_MU_CS_UPLOAD_BITMAP_MSG_ACK = 0x52430013;

const uint32_t RC_SU_CS_DOWNLOAD_BITMAP_MSG = 0x52430014;
const uint32_t RC_SU_CS_DOWNLOAD_BITMAP_MSG_ACK = 0x52430015;

const uint32_t NODE_CS_BUCKET_FINISH_MSG = 0x52430016;
const uint32_t NODE_CS_BUCKET_FINISH_MSG_ACK = 0x52430017;

#endif
