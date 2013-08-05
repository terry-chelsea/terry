/*
 * =====================================================================================
 *
 *       Filename:  request.h
 *
 *    Description:  request for test pipe and list...
 *    every request is a unsigned integer and use it to calculate the number
 *    of prime integer less than it...
 *
 *        Version:  1.0
 *        Created:  07/11/13 00:38:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_ONLY_REQUEST_AND_PRIME_H_
#define _H_ONLY_REQUEST_AND_PRIME_H_

typedef struct 
{
    int source;
    int result;
}REQ;

REQ *do_one_request(REQ *req);

REQ *create_a_request(int num);

void delete_a_request(REQ *req);

void test_requests_with_threads(int nums , int thread_num , int max_value);

void test_requests(int num , int max_value);


#endif
