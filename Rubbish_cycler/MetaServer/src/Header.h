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

//�������һЩ�����Ե��ļ�
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
    WATING = 0 ,                            //������ʱ...
    CONNECTING ,                            //���յ�����֮����δ�����������...
    WORKING ,                               //ִ��λͼ���ɹ���..
    UPLOAD ,                                //���λͼ������...
    STOPING ,
}State;

typedef struct 
{
    char *g_all_memory;                      //����λͼռ�õ��ڴ棬Ĭ��Ϊ2G�ռ䣬����һ��ȫ��ʹ��
    unsigned long block_bitmap_size;         //ÿ���ֿ�λͼ��С������CS����ȷ��
    int32_t block_num;                       //�ֿ�����Ĭ����256�����CS������ʾ���壬����CS����ȷ��
    HASH_FUNC selected_hash_func[MAX_HASH_NUMBER];     //��ǰѡ�еĹ�ϣ����...
    int32_t hash_func_num;                   //��ϣ����˵��������CS����ȷ��
    int main_to_thread[2];                   //��һ���ܵ������߳������̷߳�����Ϣ��
    int thread_to_main[2];                   //�ڶ����ܵ������߳������̷߳�����Ϣ
    int wakeup_and_block[2];                 //�������ܵ������߳������̷߳�����ϵͳ������
    State current_state;                     //ϵͳ��״̬��
    uint32_t current_bucket;                 //��ǰ���յ�Ͱ��

    int g_listen_fd;                         //�����׽���
    int send_heartbeat_fd;                   //��CS���������Ķ�ʱ���׽���
    int connectting_fd;                      //��CS�����������ӵ��׽���
    int in_connecting;                       //��ǰ�Ƿ���������������״̬��
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
