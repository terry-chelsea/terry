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
 * 可以使用以下几种方案：
 * 1、所有的位图都保存在位图中，这就需要在启动的时候一次性得分配所有的内存，暂定为集合大小为1PB，
 * 2、位图存放在磁盘上，这样就需要缓存保存需要标记的点，等到一次集中得进行标记。
 * 3、只使用两个线程，工作线程在完成一个桶的时候进行上传工作
 * 4、使用单独的线程上传位图
 * 这四个选项是1和2可以与3和4任意结合，默认情况下使用方案1和3
 * 如果指定“-c”参数表示使用第2种方案，并且需要指定缓存的大小，单位为M
 * 如果指定“-u”参数表示使用第4中方案，不需要指定参数，表示使用上传线程
 *   
 */

#define XML_FILE  "config.xml"

GLOBLE_VALUE g_global_value;

int main(int argc , char *argv[])
{
//    getopt;             //首先获取命令行参数
    
    if(read_config(XML_FILE) < 0)       //读取xml的配置信息
    {
        LOG_ERROR("Init error : read_config error!");
        goto EXIT ;
    }

    int epoll_fd = create_epoll(EPOLL_NUM);      //创建Epoll
    if(epoll_fd < 0)
    {
        LOG_ERROR("Init error : create epoll error!");
        goto EXIT ;
    }

    int listen_fd = create_listen_socket_add_epoll(NULL , xml_config.MU_listen_fd , epoll_fd);    //创建监听套接字，并加入到epoll上
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
    ret = create_pipes(epoll_fd);      //根据选项决定创建管道的个数，其中将所有的主线程的读端加入到Epoll中
    if(ret < 0)
    {
        LOG_ERROR("Init error : create pipes error!");
        goto FREE_LISTEN_FD ;
    }
    
    char *all_memroy = allocate_all_memory(xml_config.all_memory_size);   //分配所有需要的内存。
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

    if(epoll_run(epoll_fd) < 0)          //启动Epoll，系统开始运转起来。
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

