/*
 * =====================================================================================
 *
 *       Filename:  operations.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/26/13 14:59:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_OPERATIONS_H_
#define _H_OPERATIONS_H_

#include <iostream>

using namespace std;

struct Client_Req;

extern int user_login(const struct Client_Req &req);

#endif
