/*
 * =====================================================================================
 *
 *       Filename:  tableItem.cpp
 *
 *    Description:  implement of table item
 *
 *        Version:  1.0
 *        Created:  07/06/12 22:41:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com), yu
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "tableItem.h"

TableItem::TableItem(unsigned int initSize , tagRangeType start , tagRangeType end , int fd) :
    m_start(start) ,
    f_fd(fd)
{
    p_handle = new DArray(initSize);
    assert(p_handle != NULL);
    
    m_blockSize = (end - start) >> 3;
}

TableItem::~TableItem()
{
    delete p_handle;
    p_handle = NULL;

    m_start = m_blockSize = 0;
}

Ret TableItem::appendItem(const tagElemType &item)
{
    return p_handle->dAppend(item);
}

size_t TableItem::getSize()
{
    return p_handle->dGetSize();
}

Ret TableItem::flushCache(size_t &newArraySize)
{
    char *bitmapCache = new char[m_blockSize];
    assert(bitmapCache != NULL);
    memset(bitmapCache , 0 , m_blockSize);

    size_t arraySize = p_handle->dGetSize();
    size_t i = 0;
    for(i = 0 ; i < arraySize ; ++ i)
    {
        tagRangeType seq = p_handle->dGetByIndex(i) - m_start;

        andBitToBitmap(bitmapCache , m_blockSize , seq);         //this is important...
    }

    p_handle->dClearAndResize(NEW_INIT_SIZE);
    newArraySize = NEW_INIT_SIZE;

    dealWithBitmap(bitmapCache , m_blockSize);
    
    delete[] bitmapCache;
    bitmapCache = NULL;

    return SUCCEED;
}

void TableItem::andBitToBitmap(char *cache , size_t cacheSize , tagRangeType seq)
{
    size_t byteIndex = seq >> 3;
    assert(byteIndex < cacheSize);

    char *thisBytePtr = cache + byteIndex;
    int subIndex = BITS_PER_BYTE - seq % BITS_PER_BYTE - 1;

    *thisBytePtr |= (1 << subIndex);
}

void TableItem::dealWithBitmap(char *destBitmap , size_t mapSize)
{
    char *srcBitmap = new char[mapSize];
    assert(srcBitmap != NULL);
    memset(srcBitmap , 0 , mapSize);

    int offset = m_start >> 3;
    readFromFile(f_fd , offset , srcBitmap , mapSize);
    
    andBitmap(destBitmap , srcBitmap , mapSize);

    writeToFile(f_fd , offset , destBitmap , mapSize);

    delete[] srcBitmap;
    srcBitmap = NULL;
}

Ret TableItem::readFromFile(int fd , int offset , char *tempCache , int size)
{
    int readBytes = size;
    int cacheIndex = 0;
    int rlen = -1;
    bool error = false;

    if(lseek(fd , offset , SEEK_SET) == -1)
    {
        error = true;
        goto OUT;
    }
    while(readBytes > 0)
    {
        int readSize = (MAXLINE > readBytes) ? readBytes : MAXLINE;

        rlen = read(fd , tempCache + cacheIndex , readSize);
        if(rlen < 0)
        {
            perror("read data error : ");
            error = true;
            break;
        }
        else if(0 == rlen)
            break;
        else
        {
            readBytes -= rlen;
            cacheIndex += rlen;
        }
    }
OUT:
    if(error)
    {
        return FAILED;
    }
    return SUCCEED;
}

Ret TableItem::writeToFile(int fd , int offset , char *cache , int size)
{
    int writeBytes = size;
    int cacheIndex = 0;
    int wlen = -1;
    bool error = false;

    if(lseek(fd , offset , SEEK_SET) == -1)
    {
        error = true;
        goto OUT;
    }
    while(writeBytes > 0)
    {
        int writeSize = (MAXLINE > writeBytes) ? writeBytes : MAXLINE;

        wlen = write(fd , cache + cacheIndex , writeSize);
        if(wlen < 0)
        {
            perror("write data error : ");
            error = true;
            break;
        }
        else
        {
            writeBytes -= wlen;
            cacheIndex += wlen;
        }
    }
OUT:
    if(error)
    {
        return FAILED;
    }
    return SUCCEED;
}

void TableItem::andBitmap(char *dest, char *src , int size)
{
    int countByInt = size / (sizeof(unsigned long) / sizeof(char));
    int lastSize = size % (sizeof(unsigned long) / sizeof(char));

    unsigned long *destPtr = (unsigned long *)dest;
    unsigned long *srcPtr  = (unsigned long *)src;
    size_t i = 0;
    for(i = 0 ; i < countByInt ; ++ i)
    {
        destPtr[i] |= srcPtr[i];
    }

    for(i = 0 ; i < lastSize ; ++ i)
    {
        dest[size - 1 - i] |= src[size - 1 - i];
    }
}
