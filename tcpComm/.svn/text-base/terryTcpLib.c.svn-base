/*
 * =====================================================================================
 *
 *       Filename:  terryTcpLib.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/12 01:29:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "terryTcpLib.h"

char *program_name = NULL;

/* print error , based on the sign decide if exit ? based on error if print to stderr */
void error(int sign , int error , char *fmt , ...)
{
    va_list ap;

    va_start(ap , fmt);
    fprintf(stderr , "in %s : " , program_name);
    vfprintf(stderr , fmt , ap);

    va_end(ap);

    if(error)
    {
        fprintf(stderr , ": %s (%d)\n" , strerror(errno) , errno);
    }
    if(sign)
    {
        exit(sign);
    }
}

/*  */
void setAddress(char *ip , char *port , struct sockaddr_in *addr)
{
    if(NULL == addr)
        error(EXIT_ERROR , NOT_PRINT_ERRNO , "addr error\n");

    memset(addr , 0 , sizeof(*addr));
    addr->sin_family = AF_INET;
    if(NULL == ip)
    {
        addr->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        int err = inet_pton(AF_INET , ip , &(addr->sin_addr));
        if(retError(err))
            error(EXIT_ERROR , PRINT_ERRNO , "inet_pton error of ip %s : " , ip);
    }

    if(port != NULL)
    {
        addr->sin_port = htons(atoi(port));
    }
}

void setNonblock(int fd)
{
    int val = 0;
    val = fcntl(fd , F_GETFL , 0);
    if(retError(val))
        error(EXIT_ERROR , PRINT_ERRNO , "fcntl get fd %d error" , fd);

    val |= O_NONBLOCK;
    val = fcntl(fd , F_SETFL , val);
    if(retError(val))
        error(EXIT_ERROR , PRINT_ERRNO , "fcntl set fd %d error" , fd);
}

int tcpServer(char *ip , char *port)
{
    struct sockaddr_in addr;

    int fd = -1;
    const int on = 1;

    setAddress(ip , port , &addr);

    fd = socket(AF_INET , SOCK_STREAM , 0);
    if(retError(fd))
        error(EXIT_ERROR , PRINT_ERRNO , "socket error");

    if(setsockopt(fd , SOL_SOCKET , SO_REUSEADDR , (char *)&on , sizeof(on)) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "setsockopt error");

    if(bind(fd , (struct sockaddr *)&addr , sizeof(addr)) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "bind error");

    if(listen(fd , LISTENQ) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "listen error");

    return fd;
}

int tcpClient(char *ip , char *port)
{
    struct sockaddr_in addr;
    int fd = -1;

    setAddress(ip , port , &addr);

    fd = socket(AF_INET , SOCK_STREAM , 0);
    if(retError(fd))
        error(EXIT_ERROR , PRINT_ERRNO , "socket error");

    if(connect(fd , (struct sockaddr *)&addr , sizeof(addr)) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "connect error");

    return fd;
}

int createEpoll(int maxEpoll)
{
    int fd = -1;
    fd = epoll_create(maxEpoll);

    if(retError(fd))
        error(EXIT_ERROR , PRINT_ERRNO , "create epoll error");

    return fd;
}

void addToEpoll(int epollFd , int fd , int events , void *ptr)
{
    struct epoll_event event;
    memset(&event , 0 , sizeof(event));

    event.events = events;
    if(NULL == ptr)
        event.data.fd = fd;
    else
        event.data,ptr = ptr;

    if(epoll_ctl(epollFd , EPOLL_CTL_ADD , fd , &event) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "epoll_ctl add fd %d error" , fd);
}

void modifyInEpoll(int epollFd , int fd , int events)
{
    struct epoll_event event;

    event.events = events;
    if(epoll_ctl(epollFd , EPOLL_CTL_MOD , fd , &event) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "epoll_ctl modify fd %d error" , fd);
}

void deleteFromEpoll(int epollFd , int fd)
{
    if(epoll_ctl(epollFd , EPOLL_CTL_DEL , fd , NULL) < 0)
        error(EXIT_ERROR , PRINT_ERRNO , "epoll_ctl delete fd %d error" , fd);
}

int readnBlocking(int fd , char *buff , size_t len)
{
    int counter = 0;
    int readSize = 0;

    counter = len;
    while(counter > 0)
    {
        readSize = read(fd , buff , counter);
        if(retError(readSize))
        {
            if(EINTR == errno)
                continue ;
            else
            {
                error(WARNING_ERROR , PRINT_ERRNO , "read fd %d error" , fd);
                return -1;
            }
        }
        else if(0 == readSize)
            return (len - counter);
        else
        {
            buff += readSize;
            counter -= readSize;
        }
    }

    return len;
}

int readnNonBlocking(int fd , char *buff , size_t len , char *over)
{
    int counter = 0;
    int readSize = 0;

    counter = len;
    while(counter > 0)
    {
        readSize = read(fd , buff , counter);
        if(retError(readSize))
        {
            if(EINTR == errno)
                continue ;
            else if(EAGAIN == errno)
            {
                *over = 0;
                return (len - counter);
            }
            else
            {
                error(WARNING_ERROR , PRINT_ERRNO , "read fd %d error" , fd);
                return -1;
            }
        }
        else if(0 == readSize)
        {
            *over = 1;
            return (len - counter);
        }
        else
        {
            buff += readSize;
            counter -= readSize;
        }
    }

    return len;
}

int depthTraverseDir(const char *path , DO_REG_FILE dealWithReg , DO_OTH_FILE dealWithOth)
{
    char allPath[MAX_PATH_LENGTH];
    memset(allPath , 0 , MAX_PATH_LENGTH);

    int allPathLength = strlen(path);
    memcpy(allPath , path , allPathLength);
    allPath[allPathLength] = '\0';

    DIR *dir = opendir(allPath);
    if(NULL == dir)
        error(EXIT_ERROR , PRINT_ERRNO , "open directory %s error" , allPath);

    struct dirent *direntp = NULL;
    while((direntp = readdir(dir)) != NULL)
    {
        if(!memcmp(direntp->d_name , "." , sizeof(".")) || !memcmp(direntp->d_name , ".." , sizeof("..")))
            continue ;

        appendNewPath(allPath , direntp->d_name);
        if(DT_REG == direntp->d_type)
        {
            if(dealWithReg(allPath) < 0)
                error(WARNING_ERROR , NOT_PRINT_ERRNO , "deal with file %s error\n" , allPath);
        }
        else if(DT_DIR == direntp->d_type)
        {
            if(depthTraverseDir(allPath , dealWithReg , dealWithOth) < 0)
                error(WARNING_ERROR , NOT_PRINT_ERRNO , "deal with directory %s error\n" , allPath);
        }
        else 
        {
            if(dealWithOth(allPath) < 0)
                error(WARNING_ERROR , NOT_PRINT_ERRNO , "deal with other file %s error\n" , allPath);
        }

        clearNewPath(allPath);
    }

    closedir(dir);
    return 0;
}

void appendNewPath(char *destPath , char *newPath)
{
    int newSize = strlen(newPath);
    int destSize = strlen(destPath);

    /*
    if(!memcmp(destPath , ROOT_DIR , sizeof(ROOT_DIR)))
    {
        *destPath = DELIMITER_CHAR;
        memcpy(destPath + 1 , newPath , newSize);
        destPath[newSize + 1] = '\0';
        return ;
    }
    */

    destPath[destSize] = DELIMITER_CHAR;
    memcpy(destPath + destSize + 1 , newPath , newSize);
    destPath[destSize + 1 + newSize] = '\0';
}

void clearNewPath(char *path)
{
    int pathSize = strlen(path);

    char *endPtr = path + pathSize - 1;
    while((*endPtr != DELIMITER_CHAR) && (endPtr >= path))
        --endPtr;
    if(endPtr != path)
        *endPtr = '\0';
    else 
        *endPtr = DELIMITER_CHAR;
}

void dealWithInput(char *path)
{
    int pathSize = strlen(path);
    
    char *endPtr = path + pathSize - 1;
    while((*endPtr == DELIMITER_CHAR) && (endPtr > path))
        --endPtr;

    *(endPtr + 1) = '\0';
}

int widthTraverseDir(const char *path , DO_REG_FILE dealWithReg , DO_OTH_FILE dealWithOth)
{
    LIST_HEAD *pathCacheHead = (LIST_HEAD *)malloc(sizeof(LIST_HEAD));
    memset(pathCacheHead , 0 , sizeof(LIST_HEAD));
    pathCacheHead->first = NULL;
    pathCacheHead->last = NULL;
    pathCacheHead->num = 0;

    char allPath[MAX_PATH_LENGTH];
    memset(allPath , 0 , MAX_PATH_LENGTH);

    int allPathLength = strlen(path);
    memcpy(allPath , path , allPathLength);
    allPath[allPathLength] = '\0';

    PATH *temp = packageOnePath(allPath);
    addToCacheList(pathCacheHead , temp);

    PATH *getPath = NULL;

    while(!listIsEmpty(pathCacheHead))
    {
        getPath = getListHead(pathCacheHead);
        if(NULL == getPath)
        {
            printf("list Number is %d , get an NULL object , ERROR !\n" , pathCacheHead->num);
            break;
        }
        memcpy(allPath , getPath->path , strlen(getPath->path) + 1);

        DIR *dir = opendir(getPath->path);
        if(NULL == dir)
            error(EXIT_ERROR , PRINT_ERRNO , "open directory %s error" , getPath->path);

        struct dirent *direntp = NULL;
        while((direntp = readdir(dir)) != NULL)
        {
            if(!memcmp(direntp->d_name , "." , sizeof(".")) || !memcmp(direntp->d_name , ".." , sizeof("..")))
                continue ;

            appendNewPath(allPath , direntp->d_name);
            if(DT_REG == direntp->d_type)
            {
                if(dealWithReg(allPath) < 0)
                    error(WARNING_ERROR , NOT_PRINT_ERRNO , "deal with file %s error\n" , allPath);
            }
            else if(DT_DIR == direntp->d_type)
            {
                temp = packageOnePath(allPath);
                addToCacheList(pathCacheHead , temp);
            }
            else
            {
                if(dealWithOth(allPath) < 0)
                    error(WARNING_ERROR , NOT_PRINT_ERRNO , "deal woth other file %s error\n" , allPath);
            }
            clearNewPath(allPath);
        }
        free(getPath->path);
        free(getPath);
        getPath = NULL;

        closedir(dir);
    }

    free(pathCacheHead);
    pathCacheHead = NULL;

    return 0;
}

PATH *packageOnePath(char *path)
{
    PATH *pathItem = (PATH *)malloc(sizeof(PATH));
    memset(pathItem , 0 , sizeof(PATH));

    int length = strlen(path) + 1;
    pathItem->next = NULL;
    pathItem->path = (char *)malloc(length);
    memcpy(pathItem->path , path , length);

    return pathItem;
}

void addToCacheList(LIST_HEAD *head , PATH *path)
{
    if((NULL == head->first) && (NULL == head->last))
    {
        head->first = path;
        head->last = path;
    }
    else
    {
        path->next = head->last->next;
        head->last->next = path;
        head->last = path;
    }
        
    ++ head->num;
}

PATH *getListHead(LIST_HEAD *head)
{
    if(listIsEmpty(head))
        return NULL;

    PATH *headPath = head->first;
    head->first = head->first->next;

    if(NULL == head->first)
        head->last = NULL;

    -- head->num;
    return headPath;
}

int listIsEmpty(LIST_HEAD *head)
{
    return ((NULL == head->first) && (NULL == head->last));
}


