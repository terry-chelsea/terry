/*
 * =====================================================================================
 *
 *       Filename:  tableItem.h
 *
 *    Description:  one item of cache table , use two dynamic arrays , switch them if one need to be dealing with...
 *
 *        Version:  1.0
 *        Created:  07/06/12 22:21:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#ifndef _TABLE_ITEM_H_
#define _TABLE_ITEM_H_

#include<iostream>
#include "../dymanicArray/dArray.h"
#include <assert.h>
#include<unistd.h>
#include<fcntl.h>

typedef long long tagElemType;
typedef size_t    tagRangeType;
#define ARRAY_SIZE_OF_ITEM  2
#define NEW_INIT_SIZE       512
#define BITS_PER_BYTE       8
#define MAXLINE             4096

class TableItem
{
    public :
        TableItem(unsigned int , tagRangeType , tagRangeType , int);
        ~TableItem();

        Ret appendItem(const tagElemType &);
        Ret flushCache(size_t &);
        size_t getSize();
        size_t getLength();

    private:
        void andBitToBitmap(char * , size_t , tagRangeType );
        void andBitmap(char *, char *, int);
        Ret  readFromFile(int , int , char * , int);
        Ret  writeToFile(int , int , char * , int);
        void dealWithBitmap(char * , size_t );

    private :
        DArray *     p_handle;
        tagRangeType m_start;
        tagRangeType m_blockSize;
        int          f_fd;

};

#endif
