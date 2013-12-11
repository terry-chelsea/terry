/*
 * =====================================================================================
 *
 *       Filename:  Heartbeat.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 23:07:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Heartbeat.h"
#include "Epoll.h"
#include "Communication.h"

//定时器超时发送心跳信息
//并且会查看CS的心跳回复是否已经超时
int deal_with_send_heartbeat(int fd , int socket_fd , int epoll_fd)
{
    int times = 0;
    if(read_timer(fd , &times) < 0)
    {
        LOG_ERROR("In deal_with_send_heartbeat : read timer fd error : %s" , STR_ERROR);
        return -1;
    }         
    LOG_INFO_TIME("In deal_with_send_heartbeat : read heartbeat timer expired %d times" , times);

    if(times <= 0)
    {
        LOG_WARNING("In deal_with_send_heartbeat : read timer fd is 0!");
        return 0;
    }

    int check_ret = check_CS_timeout();
    if(check_ret < 0)
    {
        LOG_ERROR("In deal_with_send_heartbeat : check CS timeout error !");
        return -1;
    }
    if(check_ret)                                                  //查看CS服务器是否已经超时了
    {
        struct sockaddr_in addr;
        memset(&addr , 0 , sizeof(addr));
        if(setAddress(xml_config.CS_IP , xml_config.Alive_port , &addr) < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : setAddress error !");
            return -1;
        }
        int connect_fd = socket(AF_INET , SOCK_STREAM , 0);
        if(connect_fd < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : socket error : %s !" , STR_ERROR);
            return -1;
        }
        //非阻塞的连接，如果连接成功说明CS上没有崩溃。
        int ret = connect(connect_fd , (struct sockaddr *)&addr , sizeof(addr));
        if(ret < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : connect error : %s !" , STR_ERROR);
            return -1;
        }
        //如果连接返回0，说明连接成功，直接关闭这个套接字就可以了。
        close(connect_fd);
        //既然CS没有失效，那么就继续执行，更新时间戳？
        update_timestamp();
    }

    //无论触发了多少次，但是只发送一次心跳信息。因为心跳只会记录最后的时间
    MsgHeader head;
    memset(&head , 0 , sizeof(head));
    head.cmd = MSG_MU_CS_RUBBUSH_RECYCLE_HEARTBEAT;

    NODE *finish_list = g_global_value.finish_bucket_list;
    if(!list_is_empty(finish_list))
    {
        int bucket_nr = -1; 
        list_get_and_delete_front_node(finish_list , &bucket_nr);
        head.para1 = HEARTBEAT_ONE_BUCKET_FINISH;
        int buf_length = sizeof(MU_CS_FINISH_ONE_BUCKET_struct);
        MU_CS_FINISH_ONE_BUCKET_struct *buf = (MU_CS_FINISH_ONE_BUCKET_struct *)malloc(buf_length);
        if(NULL == buf)
        {
            LOG_ERROR("In deal_with_send_heartbeat : allocate memory error !");
            return -1;
        }
        if(add_to_send_buffer(head , buf , buf_length) < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : add send data to buffer error!");
            free(buf);
            return -1;
        }    
        free(buf);
        buf = NULL;
    }
    else if(1 == g_global_value.I_finish)
    {
        if(send_all_finish_bucket() < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : send all finish bucket error !");
            return -1;
        }
    }
    else
    {
        head.para1 = HEARTBEAT_HEARTBEAT;
        if(add_to_send_buffer(head , NULL , 0) < 0)
        {
            LOG_ERROR("In deal_with_send_heartbeat : add send data to buffer error!");
            return -1;
        }
    }
    
    if(modify_in_epoll(epoll_fd , socket_fd , EPOLLIN | EPOLLOUT))
    {
        LOG_ERROR("In deal_with_send_heartbeat : modify fd %d to EPOLLOUT error!" , socket_fd);
        return -1;
    }

    return 0;
}

int check_CS_timeout()
{
    time_t now = time(NULL);
    if((time_t)-1 == now)
    {
        LOG_ERROR("In check_CS_timeout : time error : %s !" , STR_ERROR);
        return -1;
    }

    if(now - heartbeat_agent->last_heartbeat_timestamp > CS_TIMEOUT)
        return 1;

    return 0;
}

void update_timestamp()
{
    time_t timestamp = time(NULL);
    if((time_t)-1 == timestamp)                                 //更新时间戳错误，也不返回错误
    {
        LOG_ERROR("In read_back : get time stamp error : %s" , STR_ERROR);
    }
    else
        heartbeat_agent->last_heartbeat_timestamp = timestamp;                //记录时间戳
}

int send_all_finish_bucket()
{
    MsgHeader head;
    memset(&head , 0 , sizeof(head));
    head.cmd = MSG_MU_CS_RUBBUSH_RECYCLE_HEARTBEAT;
    head.para1 = HEARTBEAT_ONE_BUCKET_FINISH;

    NODE *list_head = g_global_value.finish_bucket_list;
    if(list_is_empty(list_head))
    {
        head.length = 0;

        if(add_to_send_buffer(head , NULL , 0) < 0)
        {
            LOG_ERROR("In send_all_finish_bucket : add send data to buffer error !");
            return -1;
        }
        return 0;
    }

    BUG_ON(list_head->value <= 0);                          //此时链表不可能为空了

    int buf_length = sizeof(int32_t) * list_head->value + sizeof(MU_CS_FINISH_ALL_BUCKET_struct);
    MU_CS_FINISH_ALL_BUCKET_struct *buf = (MU_CS_FINISH_ALL_BUCKET_struct *)malloc(buf_length);
    if(NULL == buf)
    {
        LOG_ERROR("In send_all_finish_bucket : allocate memory %d Byte error !" , buf_length);
        return -1;
    }
    int32_t *bucket_array = (int32_t *)(buf + 1);                      //这里存放的是真正的负载数据
    int i = 0;
    NODE *node = list_head->next;
    for( ; node != NULL ; node = node->next)
    {
        bucket_array[i] = node->value;
        ++ i;
    }
    buf->array_size = i;                                             //实际桶号的个数
    
    if(add_to_send_buffer(head , buf , buf_length) < 0)
    {
        LOG_ERROR("In send_all_finish_bucket : add send data to buffer error !");
        free(buf);
        return -1;
    }

    free(buf);
    buf = NULL;

    return 0;
}

int add_to_send_buffer(MsgHeader head , void *data , int length)
{
    head.length = length;
    int buf_length = length + sizeof(head);
    int buf_and_struct = sizeof(Out_Buf) + buf_length;

    char *buf_and_struct_ptr = (char *)malloc(buf_and_struct);      //一块分配，前面是Out_buf，后面是要发送的数据

    if(NULL == buf_and_struct_ptr)
    {
        LOG_ERROR("In add_to_send_buffer : allocate memory for buffer error!");
        return -1;
    }
    Out_Buf *send_buf = (Out_Buf *)buf_and_struct_ptr;
    buf_and_struct_ptr += sizeof(Out_Buf);
    
    memcpy(buf_and_struct_ptr , &head , sizeof(head));              //将这个报文加到缓冲区
    if(data != NULL)
    {
        memcpy(buf_and_struct_ptr + sizeof(head) , data , length);
    }
    send_buf->req_msg =(char *)(send_buf + 1);                               //保存真正发送的数据的指针
    send_buf->length = buf_length;
    send_buf->next = NULL;
    
    add_one_to_send_buffer(send_buf);

    return 0;
}

void add_one_to_send_buffer(Out_Buf *buf)
{
    Out_Buf *head_ptr = heartbeat_agent->send_buf;

    if(NULL == head_ptr)                         //发送链表为空
    {
        head_ptr = buf;
        return ;
    }
    
    while(head_ptr->next != NULL)
        head_ptr = head_ptr->next;
    head_ptr->next = buf;
}

//该函数有三个返回值：
//返回0标志执行正确
//返回1标志CS关闭了套接字，交给上层处理
//返回-1标志套接字操作失败，关闭套接字！
int deal_with_socket_in(int socket_fd , int epoll_fd)
{
    BUG_ON(NULL == heartbeat_agent);

    int read_ret = -1;
    int is_finish = 0;
    int read_length = -1;
    int read_offset = -1;

    int init_flag = heartbeat_agent->init_flag;
    int read_header_flag = 0;
    int read_data_flag = 0;
    int close_fd_flag = 0;

    if(0 == init_flag)                                               //如果上次已经读取结束，没有任何痕迹,一次新的读取操作
    {
        read_length = sizeof(MsgHeader);
        read_offset = 0;
        read_header_flag = 1;                                        //需要读取头部
    }
    else 
    {
        if(0 == heartbeat_agent->read_all_header_flag)                      //没有读取全部的报文头
        {
            read_offset = heartbeat_agent->read_header_length;
            read_length = sizeof(MsgHeader) - read_offset;
            read_header_flag = 1;
        }
        else if(0 == heartbeat_agent->read_all_data_flag)                  //当前要么是报文头没读完，要么是数据没有读完
        {
            read_offset = heartbeat_agent->read_data_length;
            read_length = heartbeat_agent->recv_buf.header.length - read_offset;

            read_data_flag = 1;
        }
        else
        {
            LOG_WARNING("In deal_with_socket_in : did not need read header ! did not need read data !");
            return 0;
        }
    }
    if(1 == read_header_flag)                                                //需要读取报文头
    {
        read_ret = read_header(socket_fd , read_offset , read_length , &is_finish);
        if(read_ret < 0)
        {
            LOG_ERROR("In deal_with_socket_in : read header error in fd %d !" , socket_fd);
            close_fd_flag = 1;
            goto ERROR ;
        }
        else if(read_ret)
        {
            LOG_WARNING_TIME("In deal_with_socket_in : CS close heartbeat !");
            close_heartbeat_socket(socket_fd , epoll_fd);
            return 1;
        }
        if(0 == is_finish)                                   //没有读取所有的报文头
        {
            heartbeat_agent->init_flag = 1;                  //没有完成，下次不会重新读取整个报文头。
            return 0;
        }

        read_length = heartbeat_agent->recv_buf.header.length;           //需要读取的伏在数据的长度
        if(0 == read_length)                                            //如果本次没有负载数据
        {
            heartbeat_agent->recv_buf.io_buf = NULL;
            heartbeat_agent->recv_buf.length = 0;
            if(read_buf(heartbeat_agent->recv_buf , epoll_fd) < 0)
            {
                LOG_ERROR("In deal_with_socket_in : read_back error !");
                close_fd_flag = 1;
            }

            heartbeat_agent->init_flag = 0;
            return 0;
        }
        else
        {
            read_offset = 0;                                           //负载数据不是0，从头开始读取伏在数据
            read_data_flag = 1;
        }
    }
    if(1 == read_data_flag)                                                                 //不需要读报文头，不适用eles
    {
        char *io_buf_ptr = heartbeat_agent->recv_buf.io_buf;
        if(NULL == io_buf_ptr)                                                              //如果伏在数据缓冲区为创建，首先创建缓冲区。
        {
            io_buf_ptr = (char *)malloc(sizeof(char) * read_length);
            if(NULL == io_buf_ptr)
            {
                LOG_ERROR("In deal_with_socket_in : allocate memory for data error !");
                close_fd_flag = 1;
                goto ERROR ;
            }
            heartbeat_agent->recv_buf.io_buf = io_buf_ptr;
        }

        read_ret = read_data(socket_fd , read_offset , read_length , &is_finish);
        if(read_ret < 0)
        {
            LOG_ERROR("In deal_with_socket_in : read data error in fd %d !" , socket_fd);
            close_fd_flag = 1;
            goto ERROR ;
        }
        else if(read_ret)
        {
            LOG_WARNING_TIME("In deal_with_socket_in : CS close heartbeat !");
            close_heartbeat_socket(socket_fd , epoll_fd);
            return 1;
        }

        if(0 == is_finish)
        {
            heartbeat_agent->init_flag = 1;                                                 //没有读取结束，下次肯定会从数据开始读起。
            return 0;
        }
        else 
        {
            heartbeat_agent->recv_buf.length = heartbeat_agent->recv_buf.header.length;
            if(read_back(heartbeat_agent->recv_buf , epoll_fd) < 0)
            {
                LOG_ERROR("In deal_with_socket_in : read_back error !");
                close_fd_flag = 1;
            }
            free(io_buf_ptr);
            heartbeat_agent->recv_buf.io_buf = NULL;                                       //执行完成之后释放数据

            heartbeat_agent->init_flag = 0;
            return 0;
        }
    }

ERROR :
    if(1 == close_fd_flag)
    {
        close_heartbeat_socket(socket_fd , epoll_fd);
        return -1;                                                                         //所以出错都会从这里返回，已经关闭了套接字。
    }
    
    BUG_ON(1);                                                                              //根据以上的判断，一次读取操作要么需要读取头部要么需要读取数据，
                                                                                            //甚至两者都需要读取，所以不可能执行到这里.
    return 0;
}

void close_heartbeat_socket(int socket_fd , int epoll_fd)
{
    close(socket_fd);
    delete_from_epoll(epoll_fd , socket_fd);
    if(heartbeat_agent != NULL)
    {
        clear_agent();
        free(heartbeat_agent);
        heartbeat_agent = NULL;
    }
}

int read_back(In_Req req , int epoll_fd)
{
    MsgHeader header = req.header;
    char *data_buf = req.io_buf;
    int  data_length = req.length;
    
    if(MSG_MU_CS_RUBBUSH_RECYCLE_HEARTBEAT_ACK != header.cmd)
    {
        LOG_ERROR("In read_back : Undefined cmd type ! not heartbeat ack , it is %d" , header.cmd);
        return -1;
    }
    switch(header.para1)
    {
        case HEARTBEAT_BEGIN_RECYCLE :
        {
            int ret = MU_start_rubbish_recycle(req , epoll_fd);
            if(ret < 0)
            {
                LOG_ERROR("In read_back : start recycle error !");
                return -1;
            }
            break;
        }

        case HEARTBEAT_ADD_BUCKET : 
            if(MU_add_new_bucket(req) < 0)                      //添加错误，不返回错误
            {
                LOG_ERRORR("In read_back : add new bucket error !");
            }
            break;

        case HEARTBEAT_HEARTBEAT_ACK :
            if(MU_CS_heartbeat_ack(req) < 0)                    //处理心跳信息错误，不返回错误
            {
                LOG_ERRORR("In read_back : deal with heartbeat ack error !");
            }
            break;

        default :
            LOG_WARNING("In read_back : Undefined sub command ! it is %d" , header.para1);
            break;
    }

    //只要能够收到报文，就更新时间戳.
    update_timestamp();

    return 0;
}

int MU_start_rubbish_recycle(In_Req req , int epoll_fd)
{
    BUG_ON(NULL == req.io_buf);

    CS_MU_RUBBISH_RECYCLE_struct *recycle_ptr = (CS_MU_RUBBISH_RECYCLE_struct *)req.io_buf;

    if(0 == recycle_ptr->block_prefix)
        recycle_ptr->block_prefix = BLOCK_PREFIX;
    
    g_global_value.block_num = (1 << recycle_ptr->block_prefix);
    g_global_value.block_bitmap_size = recycle_ptr->bitmap_size / g_global_value.block_num;
    g_global_value.hash_func_num = recycle_ptr->hash_func_number;

    int bucket_number = recycle_ptr->array_size;
    if(bucket_number != 0)
    {
        NODE *normal_list = list_create();
        if(NULL == normal_list)
        {
            LOG_ERROR("In MU_start_rubbish_recycle : create normal list error !");
            return -1 ;
        }
        g_global_value.normal_bucket_list = normal_list;
        normal_list = list_create();
        if(NULL == normal_list)
        {
            LOG_ERROR("In MU_start_rubbish_recycle : create recovery list error !");
            goto NORMAL_LIST_ERROR ;
        }
        g_global_value.recovery_bucket_list = normal_list;
    }

    int32_t *bucket_ptr = (int32_t *)(recycle_ptr + 1);
    int i = 0;
    for(i = 0 ; i < bucket_number ; ++ i)
    {
        int bucket_nr = (int)(*(bucket_ptr + i));
        if(add_new_bucket(bucket_nr , 0) < 0)
        {
            LOG_ERROR("In read_back : add new bucket No.%d error !" , bucket_nr);
            goto RECOVERY_LIST_ERROR;
        }
    }
    if(start_rubbish_recycle(epoll_fd) < 0)                                             //启动垃圾回收，通知子线程，创建定时器。
    {
        LOG_ERROR("In read_back : start rubbish recycle error !");
        goto RECOVERY_LIST_ERROR ;
    }
    
    return 0;

RECOVERY_LIST_ERROR :
    list_destory(g_global_value.recovery_bucket_list);
    g_global_value.recovery_bucket_list = NULL;

NORMAL_LIST_ERROR :
    list_destory(g_global_value.normal_bucket_list);
    g_global_value.normal_bucket_list = NULL;

ERROR :
    return -1;
}

int add_new_bucket(int bucket_nr , int flag)
{
    if(check_bucket_recovery(bucket_nr) == 1)
    {
        if(list_append_value(g_global_value.recovery_bucket_list , bucket_nr) < 0)
        {
            LOG_ERROR("In add_new_bucket : append a new bucket No.%d recovery list error !" , bucket_nr);
            return -1;
        }
    }
    else
    {
        if(0 == flag)
        {
            if(list_append_value(g_global_value.normal_bucket_list , bucket_nr) < 0)
            {
                LOG_ERROR("In add_new_bucket : append a new bucket No.%d to normal list error !" , bucket_nr);
                return -1;
            }
        }
        else
        {
            if(list_append_value(g_global_value.append_bucket_list , bucket_nr) < 0)
            {
                LOG_ERROR("In add_new_bucket : append a new bucket No.%d to normal list error !" , bucket_nr);
                return -1;
            }
        }
    }

    return 0;
}

int start_rubbish_recycle(int epoll_fd)
{
    int thread_fd = g_global_value.main_to_thread[1];
    int start_value = START_CHAR;
    if(notice_thread_start_recycle() < 0)
    {
        LOG_ERROR("In start_rubbish_recycle : notice thread start recycle error !");
        return -1;
    }

    int heartbeat_timer_fd = create_timer(xml_config.heartbeat_interval , epoll_fd);
    if(heartbeat_timer_fd < 0)
    {
        LOG_ERROR("In start_rubbish_recycle : create heartbeat timer error! interval is %d" , xml_config.heartbeat_interval);
        return -1;
    }

    int basic_timer_fd = create_timer(1 , epoll_fd);          //这是一个基本的定时器，用于定时的查看磁盘压力和处于恢复状态的桶的状态。定时为1秒
    if(basic_timer_fd < 0)
    {
        LOG_ERROR("In start_rubbish_recycle : create basic timer error ! interval is %d" , 1);
        close(heartbeat_timer_fd);
        return -1;
    }

    g_global_value.send_heartbeat_fd = heartbeat_timer_fd;
    g_global_value.check_disk_fd = basic_timer_fd;

    return 0;
}

int MU_add_new_bucket(In_Req req)
{
    BUG_ON(req.io_buf == NULL);
    BUG_ON(g_global_value.recovery_bucket_list != NULL);

    int32_t bucket_nr = *(int32_t *)(req.io_buf);
    int error_flag = 0;

    if(add_new_bucket(bucket_nr , 1) < 0)
    {
        LOG_ERROR("In MU_add_new_bucket : add new bucket No.%d error !" , bucket_nr);
        error_flag = 1;
    }
    if(error_flag)
    {
        list_destory(g_global_value.recovery_bucket_list);
        g_global_value.recovery_bucket_list = NULL;

        list_destory(g_global_value.normal_bucket_list);
        g_global_value.normal_bucket_list = NULL;
        return -1;
    }
    else 
        return 0;
}

int MU_CS_heartbeat_ack(In_Req req)
{
    static int counter = 1;
    LOG_INFO_TIME("In MU_CS_heartbeat_ack : get CS heartbeat ack , counter : %d" , counter);
    return 0;
}

//将发送缓冲区所有的数据都发送出去，函数有两个返回值。
//返回0标志正确执行
//返回-1表示内部出错，并已经关闭了套接字。
int deal_with_socket_out(int socket_fd , int epoll_fd)
{
    BUG_ON(NULL == heartbeat_agent);

    int close_fd_flag = 0;
    if(NULL == heartbeat_agent->send_buf)
    {
        LOG_WARNING("In deal_with_socket_out : send buffer is empty !");
        return 0;
    }

    if(send_heartbeat(socket_fd) < 0)
    {
        LOG_ERROR("In deal_with_socket_out : send heartbeat error in socket %d !");
        close_fd_flag = 1;
    }
    if(1 == close_fd_flag)
    {
        close_heartbeat_socket(socket_fd , epoll_fd);
        return -1;
    }
    
    return 0;
}

int send_heartbeat(int fd)
{
    Out_Buf *buf = heartbeat_agent->send_buf; 
    Out_Buf *temp = NULL;

    while(buf != NULL)                                                   //先写出去，然后再清空列表
    {
        temp = buf;
        buf = buf->next;

        if(write_with_blocking(fd , temp->req_msg , temp->length) < 0)
        {
            LOG_ERROR("In send_heartbeat : write blocking error in socket %d!" , fd);
            return -1;
        }
    }

    buf = heartbeat_agent->send_buf;
    while(buf != NULL)                                                   //释放所以的缓冲区... 
    {
        temp = buf->next;
        free(buf);
        buf = temp;
    }
    heartbeat_agent->send_buf = NULL;

    return 0;
}

