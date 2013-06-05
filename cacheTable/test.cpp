/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  tyest cache table
 *
 *        Version:  1.0
 *        Created:  07/06/12 18:30:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com),  yu
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "cacheTable.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

#define FILE_NAME "./testFile"
#define MAXLINE    4096
#define FILE_MODE S_IRUSR | S_IWUSR
#define TEST_TIMES  10737418240
#define GLOBLE_SIZE 15904800768 

long long glo_debug = 0;

int main(int argc , char *argv[])
{
    long long globleSize = GLOBLE_SIZE;

    long long fileSize = globleSize >> 3;
    int fd = open(FILE_NAME , O_RDWR | O_CREAT | O_TRUNC , FILE_MODE);
    int writeBytes = 0;
    
    while(writeBytes < fileSize)
    {
        int size = fileSize - writeBytes;
        size = (size > MAXLINE) ? MAXLINE : size;

        char temp[MAXLINE];
        memset(temp , 0 , MAXLINE);
        write(fd , temp , size);
        writeBytes += size;
    }

    CacheTable *cacheTable = new CacheTable(globleSize , fd);

    srand(time(NULL));
    long long i = 0;
    long long times = TEST_TIMES / 1024;
    for(i = 0 ; i < times ; ++ i)
    {
        cacheTable->appendItem(rand() % globleSize);
    }

    delete cacheTable;

    return 0;
}

