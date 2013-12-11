/*
 * =====================================================================================
 *
 *       Filename:  Tool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/12 22:44:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL UESTC
 *
 * =====================================================================================
 */

#include "Tool.h"

#define STR_ERROR strerror(errno)

//每次只读取一个int类型
int read_from_thread_blocking(int fd , int *ret)
{
    int read_ret = -1;
    int read_value = -1;
RETRY_READ :
    read_ret = read(fd , &read_value , sizeof(read_value));
    if(read_ret < 0)
    {
        if(EINTR == errno)
            goto RETRY_READ ;
        else
        {
            LOG_ERROR("In read_from_thread_blocking : read error : %s" , STR_ERROR);
            return -1;
        }
    }
    else if(0 == read_ret)
    {
        LOG_WARNING_TIME("In read_from_thread_blocking : thread close pipe , fd is %d !!!" , fd);
        return 1;
    }
    else if(read_ret != sizeof(read_value))
    {
        LOG_ERROR("In read_from_thread_blocking : can not read all int size , read %d Bytes , error !" , read_ret);
        return -1;
    }

    *ret = read_value;
    return 0;
}

int read_with_blocking(int fd , char *buf , int *ret_length)
{
    int read_length = 0;
    int expect_length = *ret_length;
    int read_ret = -1;
    
RETRY_READ :
    read_ret = read(fd , buf , expect_length - read_length);            //阻塞得读，只读一次，可能没有读取到期望的数据。
    if(read_ret < 0)
    {
        if(EINTR == errno)
            goto RETRY_READ;
        else
        {
            LOG_ERROR("In read_with_nonblock : read socket fd %d error : %s!" , fd , STR_ERROR);
            return -1;
        }
    }
    else if(0 == read_ret)
    {
        LOG_INFO_TIME("In read_with_nonblock : opposite close this socket fd %d" , fd);
        return 1;
    }
    else
        read_length += read_ret;

    *ret_length = read_length;                                    //通过参数返回实际读取的字节数

    return 0;
}

int set_nonblock(int fd , SET_TYPE type)
{
    int val = 0;
    val = fcntl(fd , F_GETFL , 0);
    if(val < 0)
    {
        LOG_ERROR("fcntl get fd %d error : %s" , fd , STR_ERROR);
        return -1;
    }

    if(SET_NONBLOCK == type)
        val |= O_NONBLOCK;
    if(SET_BLOCK == type)
        val &= ~O_NONBLOCK;
    else 
        return 0;

    val = fcntl(fd , F_SETFL , val);
    if(val < 0)
    {
        LOG_ERROR("fcntl set fd %d error : %s" , fd , STR_ERROR);
        return -1;
    }
    return 0;
}

//阻塞式的写操作，只要能够返回并且不出错说明能够写完。
int write_with_blocking(int fd , void *data , int length)
{
    int write_ret = -1;

RETRY_WRITE :
    write_ret = write(fd , data , length);
    if(write_ret < 0)
    {
        if(EINTR == errno)
            goto RETRY_WRITE ;
        else
        {
            LOG_ERROR("In write_with_blocking : write socket fd %d error : %s !" , fd , STR_ERROR);
            return -1;
        }
    }

    return 0;
}

int write_to_thread_blocking(int fd , void *data , int length)
{
    return write_with_blocking(fd , data , length);
}
