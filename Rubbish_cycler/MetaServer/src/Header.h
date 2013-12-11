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
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include "../protocol/recycle_protocol.h"


#define STR_ERROR  strerror(errno) 

#define FILE_BLOCK_SIZE (512 * 1024)

#define BLOCK_PREFIX  8                                //Ĭ�ϵķֿ��ǰ׺��ʹ��infohash��ǰ���ٸ��ֽڽ��зֿ飬Ĭ��Ϊ8
#define BLOCK_NUMBER  (1 << BLOCK_PREFIX)              //Ĭ�ϵķֿ���

#define START_CHAR    -1         //�ܵ�1�������int����,������������
#define FINISH_ALL    -1         //�ܵ�2�����Ҳ��int����,��������ȫ�����
#define STOP_CHAR     -2         //�ܵ�1֪ͨ���߳���ͣ,���߳���Ҫ��ͣ
#define CONT_CHAR     'R'        //�ȵ����ַ��ż���,ͨ���ܵ�3����

#define CS_TIMEOUT    (2 * 60)                         //�ж�CS��ʱ�����ʱ�ޣ�������ʱ����û������������̽��.
#define CHECK_RECOVER_INTERVAL 10                      //�鿴���ڻָ�״̬Ͱ�ļ��ֵ

/* In header.para1 */
/* MU to CS */
#define HEARTBEAT_ONE_BUCKET_FINISH             100             //MU��CS��һ��Ͱ����ˣ���Ҫ��������
#define HEARTBEAT_ALL_BUCKET_FINISH             101             //���е�Ͱ������ˣ�һ�㲻��Ҫ��������
#define HEARTBEAT_HEARTBEAT                     102             //��ͨ��������Ϣ

/* CS to MU */
#define HEARTBEAT_BEGIN_RECYCLE                 200             //����һ���������գ�����Ϊ�����������յĲ�����������Ͱ��
#define HEARTBEAT_ADD_BUCKET                    201             //��MU׷��һ����Ҫ��������Ͱ
#define HEARTBEAT_HEARTBEAT_ACK                 202             //��ͨ�������ظ�
//��ʱ��û����CS��ͣ�������չ�����

typedef struct 
{
    char *g_all_memory;                      //����λͼռ�õ��ڴ棬Ĭ��Ϊ2G�ռ䣬����һ��ȫ��ʹ��
    unsigned long block_bitmap_size;         //ÿ���ֿ�λͼ��С������CS����ȷ��
    int32_t block_num;                       //�ֿ�����Ĭ����256�����CS������ʾ���壬����CS����ȷ��
    int32_t hash_func_num;                   //��ϣ����˵��������CS����ȷ��
    int main_to_thread[2];                   //��һ���ܵ������߳������̷߳�����Ϣ��
    int thread_to_main[2];                   //�ڶ����ܵ������߳������̷߳�����Ϣ
    int wakeup_and_block[2];                 //�������ܵ����������̵߳�����������

    //��������������̶߳���Ҫ���ʵģ��������ϸ���Ⱥ�˳�����Բ���Ҫ����
    NODE *normal_bucket_list;                //�����������յ�ʱ�����в����ڻָ�״̬��Ͱ��ӵ���������Ժ��ٸı䡣
    //����������ֻ�������̷߳���
    NODE *recovery_bucket_list;              //ÿ���ж�֮�󽫴��ڻָ�״̬��Ͱ���뵽�������
    NODE *append_bucket_list;                //�����дӻָ�״̬ת���ɹ���״̬��Ͱ���뵽����������͸����߳�
    NODE *finish_bucket_list;                //���������߳���ɵ�Ͱ���뵽��������ϣ����͸�CS

    int g_listen_fd;                         //�����׽���
    int check_disk_fd;                       //�鿴���̵��׽���
    int send_heartbeat_fd;                   //��CS���������Ķ�ʱ���׽���
    int heartbeat_ack_fd;                    //��CS�����������ӵ��׽���

    int basic_timer_counter;                 //������ʱ����ʱ���������ڼ����̺ͻظ�����
    int thread_blocking;                     //���߳��Ƿ�����������

    int I_finish;                            //���߳��Ƿ��Ѿ������һ���������յ�ȫ��������
    int in_connecting;                       //��ǰ�Ƿ���������������״̬��
}GLOBLE_VALUE;

extern GLOBLE_VALUE g_global_value;



//����Ķ�����ϵͳ�и������ݵ����ͽṹ������protobuf��֧��C���ԣ�����ֻ�ܶ���������ˡ�����������
//������Ҫʹ�����紫�ݣ����Ա���ʹ�������ڻ������������ͣ���stdint�ж��塣
//MPC --- CS
typedef struct
{
    uint64_t scale;                              //�������յĹ�ģ
}MPC_CS_RUBBISH_RECYCLE_struct;

//CS --- MU
//ע�⣬����������Ҫʹ�ò������飬���������������ʵ�ʴ��������ṹ��ĺ��棬��array_size��־�������Ĵ�С������Ԫ�ص�������int����
typedef struct 
{
    uint64_t bitmap_size;                      //bloom filter �����λͼ��С
    int32_t  hash_func_number;                 //bloom filter ����Ĺ�ϣ��������
    int32_t  block_prefix;                     //CS�����ķֿ��С��һ��Ϊ8
    int32_t  array_size;                       //���������صĴ�С����һ�����飬����������int32_t��Ԫ�ظ��������ڴˣ��������������ṹ��
}CS_MU_RUBBISH_RECYCLE_struct;

typedef struct 
{
    int32_t  bucket_nr;                        //׷�ӵ�Ͱ��
}CS_MU_ADD_ONE_BUCKET_struct;

//CS --- RU
typedef struct
{
    int32_t hash_func_number;                 //CS����Ĺ�ϣ�����ĸ���
    int32_t array_size;                       //ͬǰ���array_size 
}CS_RU_RUBBISH_RECYCLE_struct;

//MU --- CS
typedef struct 
{
    int32_t bucket_nr;                       //��ɵ�Ͱ��
}MU_CS_FINISH_ONE_BUCKET_struct;

typedef struct 
{
    int32_t array_size;                      //������ɵ�Ͱ��
}MU_CS_FINISH_ALL_BUCKET_struct;

//RU --- RU
typedef struct 
{
    int string_size;                        //һ��infohashֵ
}SU_SU_HANDLE_RESULE_struct;


#endif
