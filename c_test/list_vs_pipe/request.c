
#include "request.h"
#include <math.h>
#include <stdlib.h>

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
