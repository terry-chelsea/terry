/*
 * =====================================================================================
 *
 *       Filename:  get_odd_number.c
 *
 *    Description:  get odd counter of less than a given number...
 *
 *        Version:  1.0
 *        Created:  07/10/13 01:11:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAX_VALUE  100000
#define TEST_NUM  10000

//#define USE_REC

int is_odd(int num)
{
    int i = 0;
    int half = sqrt(num) + 1;
    for(i = 2 ; i < half ; ++ i)
    {
        if(num % i == 0)
            return 0;
    }

    return 1;
}

int get_odd_counter(int num)
{
    int i = 0;
    int counter = 1;
    for(i = 3 ; i <= num ; ++ i)
    {
        if(!(i & 0x1))
            continue ;

        if(is_odd(i))
            ++counter;
    }
    
    return counter;
}

int get_odd_number(int num)
{
    if(num == 2)
        return 1;
    else 
        return get_odd_number(num - 1) + is_odd(num);
}


void complete_all_task_in_main_thread(int nums)
{
    int i = 0;
    for(i = 0 ; i < nums ; ++ i)
    {
        int number = rand() % MAX_VALUE;
#ifndef USE_REC
        int sum = get_odd_counter(number);
#else 
        int sum = get_odd_number(number);
#endif
        (void)sum;
//        printf("Get a number : %d and odd number whis is less than it is %d\n" , number , sum);
    }
}

void *start_tasks(void *arg)
{
    long tasks = (long)arg;
    
    int i = 0;
    for(i = 0 ; i < tasks ; ++ i)
    {
        int number = rand() % MAX_VALUE;
#ifndef USE_REC
        int sum = get_odd_counter(number);
#else 
        int sum = get_odd_number(number);
#endif
        (void)sum;
    }

    return NULL;
}

#include <pthread.h>
void complete_all_task_in_threads(int nums)
{
#define THREAD_NUM  4
    int i = 0;
    pthread_t tids[THREAD_NUM];
    int each = nums / THREAD_NUM;
    long  tasks = 0;
    for(i = 0 ; i < THREAD_NUM ; ++ i)
    {
        if(0 == i)
            tasks = nums - each * (THREAD_NUM - 1);
        else 
            tasks = each;
        if(pthread_create(tids + i , NULL , start_tasks , (void *)tasks) < 0)
        {
            perror("Create thread failed : ");
            return ;
        }
    }

    for(i = 0 ; i < THREAD_NUM ; ++ i)
        pthread_join(tids[i] , NULL);
}

#include <sys/time.h>
//#define MAIN_THREAD

int main()
{
    struct timeval start , end;

    gettimeofday(&start , NULL);

#ifdef MAIN_THREAD
    complete_all_task_in_main_thread(TEST_NUM);
#else
    complete_all_task_in_threads(TEST_NUM);
#endif

    gettimeofday(&end , NULL);
    double gap = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);

    printf("\nSum cost time %lf seconds...\n" , gap / 1000000);

    return 0;
}
