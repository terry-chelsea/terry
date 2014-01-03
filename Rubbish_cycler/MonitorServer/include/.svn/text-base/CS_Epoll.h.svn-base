/*
 * =====================================================================================
 *
 *       Filename:  CS_Epoll.h
 *
 *    Description:  CS epoll , derived from base Epoll , statistics data
 *
 *        Version:  1.0
 *        Created:  11/05/12 17:21:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  terry , fengyuatad@126.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _H_CS_EPOLL_H_
#define _H_CS_EPOLL_H_

#include "../common/comm/Epoll.h"
#include "../common/sys/Singleton.h"
#include <iostream>
#include <cstring>

const int MAX_EPOLL_FD = 4096;

class CSEpoll : public Epoll , public Singleton<CSEpoll>
{
    friend class Singleton<CSEpoll> ;
    
    public : 
        CSEpoll();
        ~CSEpoll();

        virtual int doTask();

    private :
            
};




#endif
