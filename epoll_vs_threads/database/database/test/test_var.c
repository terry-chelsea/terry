/*
 * =====================================================================================
 *
 *       Filename:  test_var.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/15/13 10:24:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

int test_func(char *fmt , ...);

int main()
{
    int a = 16;
    char zero[8] = "hello";
    char f = 'F';

    int ret = test_func("hello from %c , and content is %s , Love %d\n" , f , zero , a);

    printf("length is %d\n" , ret);
    return 0;
}

int test_func(char *fmt , ...)
{
    va_list var;
    char out[256];
    va_start(var , fmt);

    int ret = vsnprintf(out , 256 , fmt , var);
    if(ret < 0)
    {
        perror("vsnprintf error :");
        return -1;
    }

    printf("after format : %s\n" , out);
    return ret;
}
