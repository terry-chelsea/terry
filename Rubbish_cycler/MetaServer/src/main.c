/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/26/12 18:54:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "Epoll.h"
#include "Socket.h"
#include "Thread.h"
#include "Header.h"
#include "Communication.h"

/*  
 * ����ʹ�����¼��ַ�����
 * 1�����е�λͼ��������λͼ�У������Ҫ��������ʱ��һ���Ե÷������е��ڴ棬�ݶ�Ϊ���ϴ�СΪ1PB��
 * 2��λͼ����ڴ����ϣ���������Ҫ���汣����Ҫ��ǵĵ㣬�ȵ�һ�μ��еý��б�ǡ�
 * 3��ֻʹ�������̣߳������߳������һ��Ͱ��ʱ������ϴ�����
 * 4��ʹ�õ������߳��ϴ�λͼ
 * ���ĸ�ѡ����1��2������3��4�����ϣ�Ĭ�������ʹ�÷���1��3
 * ���ָ����-c��������ʾʹ�õ�2�ַ�����������Ҫָ������Ĵ�С����λΪM
 * ���ָ����-u��������ʾʹ�õ�4�з���������Ҫָ����������ʾʹ���ϴ��߳�
 *   
 */

#define XML_FILE  "config.xml"

GLOBLE_VALUE g_global_value;

int main(int argc , char *argv[])
{
//    getopt;             //���Ȼ�ȡ�����в���
    
    if(read_config(XML_FILE) < 0)       //��ȡxml��������Ϣ
    {
        LOG_ERROR("Init error : read_config error!");
        goto EXIT ;
    }

    int epoll_fd = create_epoll(EPOLL_NUM);      //����Epoll
    if(epoll_fd < 0)
    {
        LOG_ERROR("Init error : create epoll error!");
        goto EXIT ;
    }

    int listen_fd = create_listen_socket_add_epoll(NULL , xml_config.MU_listen_fd , epoll_fd);    //���������׽��֣������뵽epoll��
    if(listen_fd < 0)
    {
        LOG_ERROR("Init error : create listen socket error!");
        goto FREE_EPOLL_FD ;    
    }
    else
    {
        g_global_value.g_listen_fd = listen_fd;
    }

    int ret = -1;
    ret = create_pipes(epoll_fd);      //����ѡ����������ܵ��ĸ��������н����е����̵߳Ķ��˼��뵽Epoll��
    if(ret < 0)
    {
        LOG_ERROR("Init error : create pipes error!");
        goto FREE_LISTEN_FD ;
    }
    
    char *all_memroy = allocate_all_memory(xml_config.all_memory_size);   //����������Ҫ���ڴ档
    if(NULL == all_memroy)
    {
        LOG_ERROR("Init error : allocate all memory error!");
        goto FREE_PIPE_FDS ; 
    }
    else
    {
        g_global_value.g_all_memory = all_memroy;
    }

    create_thread(&g_global_value);

    LOG_INFO_TIME("Init success !");

    if(epoll_run(epoll_fd) < 0)          //����Epoll��ϵͳ��ʼ��ת������
    {
        goto FREE_MEMORY;
    }

    return 0;

FREE_MEMORY :
    free(g_global_value.g_all_memory);
FREE_PIPE_FDS : 
    close_pipe(g_global_value.main_to_thread);
    close_pipe(g_global_value.thread_to_main);
    close_pipe(g_global_value.wakeup_and_block);
FREE_LISTEN_FD :
    close(g_global_value.g_listen_fd);
FREE_EPOLL_FD :
    close(epoll_fd);
EXIT :
    exit(-1);
}

