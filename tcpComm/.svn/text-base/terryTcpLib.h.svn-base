/*
 * =====================================================================================
 *
 *       Filename:  terryTcpLib.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/12 01:16:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#ifndef _TERRY_TCP_COMM_LIB_H_
#define _TERRY_TCP_COMM_LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<stdarg.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#define WARNING_ERROR 0
#define EXIT_ERROR    1

#define NOT_PRINT_ERRNO     0
#define PRINT_ERRNO   1

#define LISTENQ       16
#define MAX_PATH_LENGTH 256

#define DELIMITER_CHAR '/'
#define ROOT_DIR       "/"

extern char *program_name;

#define INIT()  do { \
    program_name = (strrchr(argv[0] , '/')); \
    if(program_name != NULL) \
        ++ program_name; \
    else \
        program_name = argv[0]; \
}while(0)

#define EXIT(t)   exit(t)
#define CLOSE(fd) do{ \
        if(close(fd)) \
        error(1 , error , "close error") \
        }while(0)

#define SET_ERROR(s)   (error = (s))
#define retError(ret)    (ret < 0)

typedef struct head
{
    uint32_t cmd ;
    uint32_t length;
    int32_t  para1;
    int32_t  para2;
    int32_t  error;
}MsgHeader;

typedef struct Item
{
    struct Item *next;
    char *path;
}PATH;

typedef struct list_head
{
    PATH *first;
    PATH *last;
    int num;
}LIST_HEAD;


typedef int (*DO_REG_FILE) (char *);
typedef int (*DO_OTH_FILE) (char *);


/* for TCP client or server */
extern void error(int sign , int err , char *fmt , ...);

extern void setAddress(char *ip , char * port , struct sockaddr_in *addr);

extern void setNonblock(int fd);

extern int tcpServer(char *ip , char *port);

extern int tcpClient(char *ip , char *port);


/* for epoll */
extern int createEpoll(int maxEpoll);

extern void addToEpoll(int epollFd , int fd , int events , void *ptr);

extern void modifyInEpoll(int epollFd , int fd , int events);

extern void deleteFromEpoll(int epollFd , int fd);


/* for read */
extern int readnBlocking(int fd , char *buff , size_t len );

extern int readnNonBlocking(int fd , char *buff , size_t len , char *over);


/* for file */
extern void dealWithInput(char *path);

extern int depthTraverseDir(const char *path , DO_REG_FILE , DO_OTH_FILE);

static void appendNewPath(char *dest , char *newPath);

static void clearNewPath(char *path);

extern int widthTraverseDir(const char *path , DO_REG_FILE , DO_OTH_FILE);

static PATH *packageOnePath(char *path);


/* for list */
static void addToCacheList(LIST_HEAD *head , PATH *path);

static PATH *getListHead(LIST_HEAD *head);

static int listIsEmpty(LIST_HEAD *head);

#endif

