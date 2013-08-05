/*
 * =====================================================================================
 *
 *       Filename:  listline.c
 *
 *    Description:  use queue to send request and ack...
 *    send request to calculate number(only CPU operations),use list to send
 *    source data and get result too...
 *    use eventfd to implement notify main thread...
 *    every 100 requests read once with nonblocking mode...
 *
 *        Version:  1.0
 *        Created:  07/11/13 00:25:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "normal_list.h"
#include "request.h"
#include <stdlib.h>
#include <sys/eventfd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>

static LIST *send_list = NULL;   //use this list send request to work threads...
static LIST *recv_list = NULL;   //use this list send result to main thread...
static int  _eventfd = -1;       //use this eventfd implement notify/wait ...
static int send_request = 0;     //debug info : all send requests...
static int recv_request = 0;     //debug info : all recv requests...

#define CALCULATE_TIME(start , end) do{ \
    double gap = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec); \
    double msec = gap / 1000; \
    printf("From start to end , Cost %lf millseconds\n" , msec); \
}while(0)

#define READ_INTERVAL 100
#define MAX_NUMBER 100000
#define MAX_REQUEST 10000
#define THREAD_NUM  4

#ifdef DEBUG_IN
int requests = 0;
double all_time = 0.0;
pthread_mutex_t debug_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void *start_task(void *arg);
int check_results(int fiags);
int main()
{
#ifdef DEBUG
#ifdef MTHREAD
    test_requests_with_threads(MAX_REQUEST , THREAD_NUM , MAX_NUMBER);
#else
    test_requests(MAX_REQUEST , MAX_NUMBER);
#endif
    return 0;
#endif
    struct timeval start , end;
    gettimeofday(&start , NULL);

    send_list = list_create();
    recv_list = list_create();

    if(NULL == send_list || NULL == recv_list)
        return -1;

    //eventfd created with blocking...
    if((_eventfd = eventfd(0 , 0)) < 0)
    {
        perror("Create eventfd failed : ");
        return -1;
    }

    pthread_t tids[THREAD_NUM];
    int i = 0;
    for(i = 0 ; i < THREAD_NUM ; ++ i)
    {
        if(pthread_create(tids + i , NULL , start_task , NULL) < 0)
        {
            perror("Create thread failed : ");
            return -1;
        }
    }

    //do read operation every send 100 request...
    for(i = 1 ; i <= MAX_REQUEST ; ++ i)
    {
        if(!(i % READ_INTERVAL))
        {
            //max deal with 100 acks once a time...
            int ret = check_results(0);
            if(ret < 0)
            {
                printf("Something wrong while get results...\n");
                return -1;
            }
#ifdef DEBUG_IN
            printf("get result : %d\n" , ret);
#endif

            recv_request += ret;
        }
        
        REQ *req = create_a_request(rand() % MAX_NUMBER);
        if(NULL == req)
        {
            printf("Create request failed !\n");
            return -1;
        }
        
        if(list_put(send_list , (void *)req) == FAILED)
        {
            printf("Put a request to list failed !\n");
            return -1;
        }

        ++ send_request;
    }
#ifdef DEBUG_IN
    gettimeofday(&end , NULL);
    CALCULATE_TIME(start , end);
    printf("send %d and recv result %d\n" , send_request , recv_request);

    gettimeofday(&start , NULL);
#endif

    while(recv_request != send_request)
    {
        int ret = check_results(1);
        if(ret < 0)
        {
            printf("Last check results failed ...\n");
            return -1;
        }

        recv_request += ret;

#ifdef DEBUG_IN
        printf("send %d and recv %d\n" , send_request , recv_request);
        list_state(recv_list);
#endif
    }

    list_destory(&send_list);
    list_destory(&recv_list);
    close(_eventfd);
    _eventfd = 0;

#ifdef DEBUG_IN
    printf("all requests : %d and all cost time : %lf\n" , requests , all_time);
    gettimeofday(&end , NULL);
    CALCULATE_TIME(start , end);
#else 
    gettimeofday(&end , NULL);
    double gap = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

    printf("%lf\n" , gap / 1000000);
#endif


    return 0;
}

void *start_task(void *arg)
{
    pthread_detach(pthread_self());

    while(1)
    {
        void *elem = NULL;
        if(list_get(send_list , &elem) == FAILED)
        {
            printf("Work thread get element failed...\n");
            continue ;
        }

        REQ *req = (REQ *)elem;

#ifdef DEBUG_IN
        struct timeval start , end;
        gettimeofday(&start , NULL);
        req = do_one_request(req);
        gettimeofday(&end , NULL);

        double gap = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
        pthread_mutex_lock(&debug_mutex);
        all_time += gap / 1000;
        ++ requests;
        pthread_mutex_unlock(&debug_mutex);
#else
        req = do_one_request(req);
#endif

        if(list_put(recv_list , req) == FAILED)
        {
            printf("Work thread put elemenet failed \n");
            break;
        }

        uint64_t one = 1;
        if(write(_eventfd , &one , sizeof(one)) != sizeof(one))
        {
            perror("Write counter to eventfd failed : ");
            break;
        }
    }

    return NULL;
}

void deal_with_result(REQ *req)
{
    delete_a_request(req);
    req = NULL;
}

int do_recv_requeats(int flags)
{
    int max_number = -1;
    void *elem = NULL;

    if(!flags)
    {
        int i = 0;
        for(i = 0 ; i < READ_INTERVAL ; ++ i)
        {
            if(list_get(recv_list , &elem) == FAILED)
            {
                printf("Get a element from recv list failed !\n");
                return -1;
            }

            REQ *req = (REQ *)elem;
            deal_with_result(req);
        }

        return READ_INTERVAL;
    }
    else if(2 == flags)
        max_number = READ_INTERVAL;

    int counter = 0;
    while(max_number > 0 && counter < max_number)
    {
        if(list_get_nonblock(recv_list , &elem) == FAILED)
            break;

        REQ *req = (REQ *)elem;
        deal_with_result(req);

        ++counter;
    }

    return counter;
}

int check_results(int flags)
{
    static int last_counter = 0;
    uint64_t counter = 0;
    
    if(last_counter > READ_INTERVAL)
        return do_recv_requeats(flags);

    if(read(_eventfd , &counter , sizeof(counter)) < 0)
    {
        if(EAGAIN == errno)
        {
            printf("Now this eventfd is empty , equal to zero...\n");
            sleep(1);
            return 0;
        }
        perror("read from eventfd failed : ");
        return -1;
    }

    if(counter > READ_INTERVAL)
        last_counter += counter - READ_INTERVAL;
    else 
        flags = 2;

    return do_recv_requeats(flags);
}
