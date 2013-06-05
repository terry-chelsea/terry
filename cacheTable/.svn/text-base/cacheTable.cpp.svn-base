/*
 * =====================================================================================
 *
 *       Filename:  cacheTable.cpp
 *
 *    Description:  implement of cache table
 *
 *        Version:  1.0
 *        Created:  07/06/12 09:14:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com),  yu 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "./cacheTable.h"
#define DEBUG

extern long long glo_debug;

CacheTable::CacheTable(long long globleSize , int fd) :
    p_handle(NULL) ,
    m_globleSize(globleSize) , 
    m_sumSize(0) ,
    f_fd(fd)
{
    /*  
    m_blockSize = globleSize / DEFAULT_BLOCK_NUM;
    if(m_blockSize > MAX_BLOCK_RANGE)
    {
        m_blockSize = MAX_BLOCK_RANGE;
    }

    if(m_blockSize % BITS_PER_BYTE != 0)
    {
        m_blockSize += BITS_PER_BYTE - m_blockSize % BITS_PER_BYTE;
    }
    m_blockBytes = m_blockSize >> 3;

    if(m_blockBytes % ALIGN_BYTES != 0)
    {
        m_blockBytes += ALIGN_BYTES - m_blockBytes % ALIGN_BYTES;
        m_blockSize = m_blockBytes << 3;
    }
    */
    m_blockSize = MAX_BLOCK_RANGE;
    m_blockBytes = m_blockSize >> 3;

    m_cacheSize = (size_t)(globleSize / m_blockSize) + 1;

    p_handle = new TableItem *[m_cacheSize];
    assert(p_handle != NULL);

    size_t i = 0;
    for(i = 0 ; i < m_cacheSize ; ++ i)
    {
        TableItem *item = new TableItem(INIT_SIZE , i * m_blockSize , (i + 1) * m_blockSize , f_fd);
        assert(item != NULL);
        p_handle[i] = item;
    }
}

CacheTable::~CacheTable()
{
    flushCache();
    size_t i = 0;
    for(i = 0 ; i < m_cacheSize ; ++ i)
    {
        delete p_handle[i];
    }
    delete[] p_handle;

    p_handle = NULL;
    m_globleSize = m_blockSize = m_cacheSize = 0;
}

Ret CacheTable::appendItem(tagElemType seq)
{
    size_t index = (size_t)(seq / m_blockSize);

    TableItem *item = p_handle[index];

    if(FAILED == item->appendItem(seq))
        goto APPEND_FAILED;
    else
    {
        ++ m_sumSize;

#ifdef DEBUG
        ++ glo_debug;
#endif

        if(needFlush() && (flushCache() == SUCCEED))
            goto APPEND_SUCCEDD;
        else
            goto APPEND_FAILED;
    }
   
APPEND_SUCCEDD :
    return SUCCEED;
APPEND_FAILED :
        return FAILED;
}

bool CacheTable::needFlush()
{
    return m_sumSize > LIMIT_SIZE;
}

Ret CacheTable::flushCache()
{
    size_t i = 0;

    for(i = 0 ; i < m_cacheSize ; ++ i)
    {
        TableItem *item = p_handle[i];

        size_t oldLength = item->getSize();
        size_t newLength = 0;
        item->flushCache(newLength);

        m_sumSize -= oldLength;
    }

#ifdef DEBUG
    cout<<"flush once..."<<"times : "<<glo_debug<<endl;
#endif

    return SUCCEED;
}

/* 
Ret CacheTable::flushOneCache(char *cache , size_t cacheSize , size_t index)
{
    memset(cache , 0 , cacheSize);
    DArray *thisArray = p_handle[index];

    size_t arraySize = thisArray->dGetSize();
    size_t arrayLength = thisArray->dGetLength();
    size_t i = 0;
    for(i = 0 ; i < arraySize ; ++ i)
    {
        int cacheSeq = thisArray->dGetByIndex(i) - index * m_blockSize;
        
        andBitToBitmap(cache , cacheSize , cacheSeq);       //this is important...
    }

    char *tempCache = new char[cacheSize];
    assert(tempCache != NULL);
    memset(tempCache , 0 , cacheSize);

    Ret ret = readFromFile(f_fd , index * m_blockBytes , tempCache , cacheSize);
    if(FAILED == ret)
    {
        goto FLUSH_ERROR;
    }

    andBitmap(cache , tempCache , cacheSize);

    ret = writeToFile(f_fd , index * m_blockBytes , cache , cacheSize);
    if(FAILED == ret)
    {
        goto FLUSH_ERROR;
    }

    m_sumSize -= arrayLength;
    thisArray->dClearAndResize(INIT_SIZE);
    m_sumSize += thisArray->dGetLength();
    delete[] tempCache;
    tempCache = NULL;
    return SUCCEED;

FLUSH_ERROR :
    delete[] tempCache;
    tempCache = NULL;
    return FAILED;
}

void CacheTable::andBitToBitmap(char *cache , size_t cacheSize , int seq)
{
    size_t byteIndex = seq >> 3;
    assert(byteIndex < cacheSize);

    char *thisBytePtr = cache + byteIndex;
    int subIndex = BITS_PER_BYTE - seq % BITS_PER_BYTE - 1;

    *thisBytePtr |= (1 << subIndex);
}

Ret CacheTable::readFromFile(int fd , int offset , char *tempCache , int size)
{
    int readBytes = 0;
    int rlen = -1;
    bool error = false;

    if(lseek(fd , offset , SEEK_SET) == -1)
    {
        error = true;
        goto OUT;
    }
    while(readBytes < size)
    {
        rlen = read(fd , tempCache + readBytes , MAXLINE);
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
            readBytes += rlen;
        }
    }
OUT:
    if(error)
    {
        return FAILED;
    }
    return SUCCEED;
}

Ret CacheTable::writeToFile(int fd , int offset , char *cache , int size)
{
    int writeBytes = 0;
    int wlen = -1;
    bool error = false;

    if(lseek(fd , offset , SEEK_SET) == -1)
    {
        error = true;
        goto OUT;
    }
    while(writeBytes < size)
    {
        wlen = write(fd , cache + writeBytes , MAXLINE);
        if(wlen < 0)
        {
            perror("write data error : ");
            error = true;
            break;
        }
        else
        {
            writeBytes += wlen;
        }
    }
OUT:
    if(error)
    {
        return FAILED;
    }
    return SUCCEED;
}

void CacheTable::andBitmap(char *dest, char *src , int size)
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

*/
