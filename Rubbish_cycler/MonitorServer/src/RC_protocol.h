/*
 * =====================================================================================
 *
 *       Filename:  RC_protocol.h
 *
 *    Description:  protocol and struct for rubbish recycler...
 *
 *        Version:  1.0
 *        Created:  12/05/13 21:27:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */


#ifndef _H_RUBBISH_RECYCLER_PROTOCOL_H_
#define _H_RUBBISH_RECYCLER_PROTOCOL_H_

#include <inttypes.h>

//0x5243 = RC in ASCII...
//初始化节点，这个命令MU和SU都使用...
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

//心跳信息...
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
