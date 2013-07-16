
#include "request.h"
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

static int is_odd(int num)
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

#ifndef USE_REC
static int get_odd_counter(int num)
{
    if(num < 2)
        return 0;

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

#else
static int get_odd_counter(int num)
{
    if(num < 2)
        return 0;

    if(num == 2)
        return 1;
    else 
        return get_odd_counter(num - 1) + is_odd(num);
}
#endif

REQ *do_one_request(REQ *req)
{
    req->result = get_odd_counter(req->source);

    return req;
}


REQ *create_a_request(int num)
{
    REQ *req = (REQ *)malloc(sizeof(REQ));
    if(NULL == req)
        return NULL;

    req->source = num;
    req->result = 0;

    return req;
}

void delete_a_request(REQ *req)
{
    if(NULL != req)
        free(req);
}

//just test how long it will takes in a single thread...
void test_requests(int num , int max_value)
{
    struct timeval start , end;
    gettimeofday(&start , NULL);

    int i = 0;
    for(i = 0 ; i < num ; ++ i)
    {
        REQ *req = create_a_request(rand() % max_value);
        do_one_request(req);
        delete_a_request(req);
        req = NULL;
    }
    gettimeofday(&end , NULL);
    double gap = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    
    printf("%lf\n" , gap / 1000000);
}

