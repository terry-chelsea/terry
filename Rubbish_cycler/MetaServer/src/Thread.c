/*
 * =====================================================================================
 *
 *       Filename:  Thread.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/28/12 17:38:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Thread.h"
#include "Communication.h"
#include "Traverse.h"

extern char full_path[PATH_LENGTH];

extern GLOBLE_VALUE g_global_value;
#define BUCKET_NAME_PREFIX "bucker_"

int create_thread()
{
    pthread_t tid;
    if(pthread_create(&tid , NULL , thread_run , NULL) < 0)
    {
        LOG_ERROR("In create_thread : create thread error!");
        return -1;
    }

    LOG_INFO_TIME("Create new thread success !");

    return 0;
}

void *thread_run(void *arg)
{
    if(pthread_detach(pthread_self()) < 0)
    {
        LOG_WARNING("In child thread : thread detach error!");
    }

    int main_to_thread_read_fd = -1;
    int read_ret = -1;
RE_START:
    main_to_thread_read_fd = g_global_value.main_to_thread[0];
    read_ret = read_start_cmd(main_to_thread_read_fd);
    if(read_ret < 0)                           //出错
        thread_error_exit();
    else if(read_ret)                          //主线程关闭描述符
        thread_ok_exit();
    else
    {
        LOG_INFO_TIME("Thread start Recycle!");
    }

    if(do_thread_work() < 0)          //除非出现致命性错误，才会返回-1。
    {
        thread_error_exit();
    }
    else
        goto RE_START;
}

int do_thread_work()                             //只有在不可继续的错误时才返回-1
{
    NODE *head = g_global_value.normal_bucket_list;

    createHashFuncObj(g_global_value.hash_func_num);
    if(do_every_bucket(head) < 0)
    {
        LOG_ERROR("In do_thread_work : do_every_bucket error : pipe error !");
        thread_error_exit();
    }

    while(!list_is_empty(append_list))
    {
        if(NULL == append_list_backup)
        {
            append_list_backup = list_create();
            if(NULL == append_list_backup)
            {
                LOG_ERROR("In do_thread_work : create append_list_backup error");
                break;
            }
        }
        list_move(append_list , append_list_backup);

        if(do_every_bucket(append_list_backup) < 0)
        {
            LOG_ERROR("In do_thread_work : do_every_bucket of append list error : pipe error !");
            thread_error_exit();
        }
    }

    sleep(10);                                                          //这里阻塞一会在发送,呼呼~~~
    if(notice_main(FINISH_ALL) < 0)
    {
        LOG_ERROR("In do_thread_work : notice main all bucket finished error !");
        thread_error_exit();
    }
    int read_fd = (g_global_value.main_to_thread)[0];
    if(set_nonblock(read_fd , SET_BLOCK) < 0)          //完成一次回收，将这个套接字设置为阻塞。重新等待主线程任务
    {
        LOG_ERROR("In child thread : set pipe fd %d block error!" , read_fd);
        return -1;
    }

    return 0;
}

int do_every_bucket(NODE *head)
{
    NODE *node_entry = head->next;                    //第一个数据节点
    for( ; node_entry != NULL ; node_entry = node_entry->next)          //遍历每一个桶
    {
        int bucket_nr = node_entry->value;
        LOG_INFO("Start traverse bucket %d" , bucket_nr);

        if(traverse_bucket(bucket_nr) < 0)               
        {
            LOG_ERROR("In do_every_bucket : traverse_bucket %d error!" , bucket_nr);
            continue ;
        }
        else
        {
            LOG_INFO("Traverse bucket %d success!" , bucket_nr);
            if(finish_one_bucket(bucket_nr) < 0)
            {
                LOG_ERROR("In do_every_bucket : deal with  bucket %d finish error!" , bucket_nr);
                return -1;
            }
        }
    }

    return 0;
}

int finish_one_bucket(int bucket_nr)
{
    LOG_WARNING_TIME("In child thread : finish traverse bucket No.%d" , bucket_nr);
    if(notice_main(bucket_nr) < 0)
    {
        LOG_ERROR("In finish_one_bucket : notice main thread bucket %d finish error!" , bucket_nr);
        return -1;
    }

    return 0;
}

int notice_main(int bucket_nr)
{
    int write_fd = g_global_value.thread_to_main[1];
    int write_ret = -1;

RE_WRITE :
    write_ret = write(write_fd , &bucket_nr , sizeof(bucket_nr));
    if(write_ret < 0)
    {
        if(EINTR == errno)
            goto RE_WRITE ;
        else
        {
            LOG_ERROR("iN notice_main : write bucket number %d error : %s" , bucket_nr , strerror(errno));
            return -1;
        }
    }
    else
    {
        if(bucket_nr == FINISH_ALL)
            LOG_INFO_TIME("all bucket finished!");
        else
            LOG_INFO("write bucket number %d success to main thread" , bucket_nr);
    }
    return 0;
}


int read_start_cmd(int read_fd)
{
    int read_ret = -1;
    int value = 0;
    while(1)
    { 
        read_ret = read(read_fd , &value , sizeof(value));
        if(read_ret < 0)
        {
            if(EINTR == errno)
                continue ;
            else
            {
                LOG_ERROR("In read_start_cmd : read pipe fd %d error : %s" , read_fd , strerror(errno));
                return -1;
            }
        }
        else if(0 == read_ret)
        {
            LOG_WARNING("In read_start_cmd : main thread close pipe fd %d !" , read_fd);
            return 1;
        }
        else if(value != START_CHAR)
            continue ;
        else 
        {
            if(set_nonblock(read_fd , SET_NONBLOCK) < 0)        //读取成功，将这个套接字设置为非阻塞。
            {
                LOG_ERROR("In child thread : set pipe fd %d nonblock error!" , read_fd);
                return -1;
            }
            break;
        }
    }

    return 0;
}

void generate_bucket_dir_name(char *name , int max_length , int bucket_nr)
{
    snprintf(name , max_length , "%s%d" , BUCKET_NAME_PREFIX , bucket_nr);
}

int traverse_bucket(int bucket_nr)
{
    memset(full_path , 0 , sizeof(full_path));

    generate_bucket_dir_name(full_path , sizeof(full_path) , bucket_nr);                    //得到桶的根目录名

    if(traverse_top_dir(full_path , deal_with_regular_file) < 0)
    {
        LOG_ERROR("In traverse_bucket : traverse bucket %d top directory error !" , bucket_nr);
        return -1;
    }

    return 0;
}

void thread_error_exit()
{
    pthread_exit((void *)-1);
}

void thread_ok_exit()
{
    pthread_exit((void *)0);
}

int get_append_bucket(int bucket_nr)
{
    if(NULL == append_list)
    {
        append_list = list_create();
        if(NULL == append_list)
        {
            LOG_ERROR("In get_append_bucket : create append list error !");
            return -1;
        }
    }
    else 
    {
        if(list_preappend_value(append_list , bucket_nr) < 0)
        {
            LOG_ERROR("In get_append_bucket : add new bucket error !");
            return -1;
        }
    }
    return 0;
}

//函数只会在读操作出错的时候才会返回-1，其他情况返回0，或者直接退出。
int deal_with_regular_file(char *file_name)
{
    int new_bucket = -1;
    int ret = thread_check_waiting(&new_bucket);
    if(ret < 0)
    {
        LOG_ERROR("In deal_with_regular_file : check thread waiting error !");
        return -1;
    }
    else if(1 == ret)
    {
        LOG_WARNING_TIME("In deal_with_regular_file : main close wakeup and block pipe !!!");
        return -1;
    }
    else                                   //不需要阻塞
    {
        if(new_bucket != -1 && (get_append_bucket(new_bucket) < 0))
        {
            LOG_ERROR("In deal_with_regular_file : add append bucket error !!!");
        }
        if(real_deal_with_regular_file(file_name) < 0)
            LOG_ERROR("In deal_with_regular_file : deal with file %s error!" , file_name);
        return 0;                                //始终返回0.
    }
    
    //需要阻塞
    ret = thread_wait_main_wakeup();
    if(ret < 0)
    {
        LOG_ERROR("In deal_with_regular_file : thread wait main wakeup error !");
        return -1;
    }
    else if(1 == ret)
    {
        LOG_WARNING_TIME("In deal_with_regular_file : main close wakeup and block pipe !!!");
        return -1;
    }
    

    if(real_deal_with_regular_file(file_name) < 0)
        LOG_ERROR("In deal_with_regular_file : deal with file %s error!" , file_name);

    return 0;
}

int real_deal_with_regular_file(char *file_name)
{
    int fd = open(file_name , O_RDONLY);
    if(fd < 0)
    {
        LOG_ERROR("In real_deal_with_regular_file : open file %s error !" , file_name);
        return -1;
    }

    int file_length = get_file_length(fd);
    if(file_length < 0)
    {
        LOG_ERROR("In real_deal_with_regular_file : get file %s length error !" , file_name);
        close(fd);
        return -1;
    }
    if(0 == file_length)
    {
        LOG_WARNING("In real_deal_with_regular_file : get file %s length is 0!" , file_name);
        close(fd);
        return 0;
    }
    
    int block_number = file_length / BLOCK_SIZE;
    if(file_length % BLOCK_SIZE)
        ++ block_number;

    int buffer_length = block_number * INFOHASH_LENGTH;
    char *infohash_buffer = (char *)malloc(buffer_length);
    if(NULL == infohash_buffer)
    {
        LOG_ERROR("In real_deal_with_regular_file : allocate memory failed!");
        close(fd);
        return -1;
    }
    if(read(fd , infohash_buffer , buffer_length) != buffer_length)
    {
        LOG_ERROR("In real_deal_with_regular_file : can not read all infohash in file %s!" , file_name);
        close(fd);
        free(infohash_buffer);
        return -1;
    }

    int i = 0;
    for(i = 0 ; i < block_number ; ++ i)
    {
        char *infohash = infohash_buffer + i * INFOHASH_LENGTH;
        int  bitmap_block_size = g_global_value.block_bitmap_size;
        int  block_nr = infohash[0];                          //第一个字符作为标记
        char *bitmap_block_buffer = g_global_value.g_all_memory + (block_nr * bitmap_block_size);
   
        deal_with_infohash_with_all_buffer(bitmap_block_buffer , bitmap_block_size , infohash , INFOHASH_LENGTH);
    }

    close(fd);
    free(infohash_buffer);
    return 0;
}

int get_file_length(int fd)
{
    return (BLOCK_SIZE * 10);
}

