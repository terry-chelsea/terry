/*
 * =====================================================================================
 *
 *       Filename:  test_time.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/19/13 15:53:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <sys/time.h>
#include <stdio.h>


int main()
{
    struct timeval now;
    time_t nw = -1;

    gettimeofday(&now , NULL);
    nw = time(NULL);

    printf("timeofday : %d and time : %d\n" , now.tv_sec , nw);

    return 0;
}
