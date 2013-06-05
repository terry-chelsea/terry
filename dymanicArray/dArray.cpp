/*
 * =====================================================================================
 *
 *       Filename:  dArray.cpp
 *
 *    Description:  implement of dymanic array
 *
 *        Version:  1.0
 *        Created:  07/05/12 22:13:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com),yu 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "dArray.h"
#include<assert.h>

DArray::DArray(size_t size) :
    p_base(NULL) , 
    m_length(size) ,
    m_size(0)
{
    p_base = new tagElemType[m_length];
    assert(p_base != NULL);

    memset(p_base , 0 , sizeof(tagElemType) * m_length);
}

//create a dymanic array from an existing array , assume source array vaild from index 0 to validLength.
DArray::DArray(tagElemType *src , size_t sLength , size_t validLength) :
    p_base(NULL) , 
    m_length(validLength) , 
    m_size(validLength)
{
    p_base = new tagElemType[validLength];
    assert(p_base != NULL);

    memcpy(p_base , src , validLength * sizeof(tagElemType));
}

DArray::~DArray()
{
    assert(p_base != NULL);
    delete []p_base;
    p_base = NULL;
    m_length = m_size = 0;
}

//insert an item in given index , index range from 0 to m_size - 1
Ret DArray::dInsert(size_t index , const tagElemType & data)
{
    size_t i = m_size;
    if(needExpand())
    {
        Ret ret = dExpand();
        if(FAILED == ret)
            goto INSERT_FAILED;
    }

    if(index >= m_size)
    {
        p_base[m_size] = data;
        goto INSERT_SUCCEED;
    }

    for( ; i > index ; -- i)
    {
        p_base[i] = p_base[i - 1];
    }
    p_base[i] = data;

INSERT_SUCCEED:
    m_size ++;

    return SUCCEED;
INSERT_FAILED:
    return FAILED;
}

Ret DArray::dPrepend(const tagElemType & data)
{
    return dInsert(0 , data);
}

Ret DArray::dAppend(const tagElemType & data)
{
    return dInsert(m_size , data);
}

Ret DArray::dDelete(size_t index , tagElemType *value)
{
    bool retFlag = true;
    size_t i = index;
    if(NULL == value)
        retFlag = false;

    if(index >= m_size || index < 0)
        goto DELETE_FAILED;

    if(retFlag)
        *value = p_base[index];

    for( ; i < m_size ; ++ i)
    {
        p_base[i] = p_base[i + 1];
    }
    m_size --;

#ifndef _DO_NOT_SHRINK_
    if(needShrink())
        dShrink();
#endif

    return SUCCEED;
DELETE_FAILED:
    return FAILED;
}

Ret DArray::dDelete(size_t index)
{
    return dDelete(index , NULL);
}

tagElemType & DArray::dGetByIndex(size_t index)
{
    assert(index >= 0 && index < m_size);
    return p_base[index];
}

Ret DArray::dSetByIndex(size_t index , const tagElemType & data)
{
    assert(index >= 0 && index < m_length);
    if(index >= m_size)
    {
        p_base[m_size - 1] = data;
    }
    else
    {
        p_base[index] = data;
    }

    return SUCCEED;
}

size_t DArray::dGetSize()
{
    return m_size;
}

size_t DArray::dGetLength()
{
    return m_length;
}

Ret DArray::dClearAndResize(int size)
{
    assert(size > 0);
    m_length = size;
    m_size = 0;
    delete[] p_base;
    p_base = NULL;

    p_base = new tagElemType[m_length];
     
    return SUCCEED;
}

bool DArray::needExpand()
{
    return (m_length - m_size <= MIN_SPACE);
}

bool DArray::needShrink()
{
    return (m_length - m_size >= (m_length >> 2));
}

size_t DArray::getSpaceSize()
{
    return m_length - m_size;
}

//expand dynamic array if need , expand 1.5 times of original array size.
Ret DArray::dExpand()
{
#ifdef DEBUG
    printf("before expand length is %d , " , m_length);
#endif
    if(m_length < MIN_EXPAND_LENGTH)
    {
        m_length = DEFAULT_LENGTH;
    }
    else
    {
        m_length = m_length + (m_length >> 1);  //add operator is higher priority than diaplace operator
    }
#ifdef DEBUG
    printf("after expand length is %d\n" , m_length);
#endif

    tagElemType *newBase = new tagElemType[m_length];
    if(NULL == newBase)
        goto EXPAND_FAILED;

    memcpy(newBase , p_base , sizeof(tagElemType) * m_size);
    delete[] p_base;
    p_base = newBase;

    return SUCCEED;
EXPAND_FAILED:
    if(newBase != NULL)
        delete[] newBase;
    return FAILED;
}

//shrink when need , shrink 0.5 times of original array space size.
Ret DArray::dShrink()
{
#ifdef DEBUG
    printf("before shrink length is %d , " , m_length);
#endif
    size_t spaceSize = getSpaceSize();
    m_length = m_length - (spaceSize >> 1);
#ifdef DEBUG
    printf("after shrink length is %d\n" , m_length);
#endif

    tagElemType *newBase = NULL;
    if(m_length <= DEFAULT_LENGTH)
        goto SHRINK_SUCCEED;

    newBase = new tagElemType[m_length];
    if(NULL == newBase)
        goto SHRINK_FAILED;

    memcpy(newBase , p_base , sizeof(tagElemType) * m_size);
    delete[] p_base;
    p_base = newBase;

SHRINK_SUCCEED:
    return SUCCEED;
SHRINK_FAILED:
    if(newBase != NULL)
        delete[] newBase;
    return FAILED;
}

/*  
//find items that satified condition provided by compare function , return items number.
int dArray::dFind(CompareFunc comp , void *ctx)
{
    int count = 0;

    size_t i = 0;
    for( ; i < m_size ; ++ i)
    {
        if(comp(ctx , &p_base[i]))
            count ++;
    }

    return count;
}*/

//find the first item that satified condition provided by compara function.
size_t DArray::dFind(CompareFunc comp , void *ctx)
{
    size_t i = 0;
    for( ; i < m_size ; ++ i)
    {
        if(comp(ctx , &p_base[i]))
            break;
    }
    if(i == m_size)
        i = -1;

    return i;
}

Ret DArray::dForeach(VisitFunc visit , void *ctx)
{
    size_t i = 0;
    for( ; i < m_size ; ++ i)
    {
        visit(ctx , &p_base[i]);
    }

    return SUCCEED;
}

Ret DArray::dSort(SortFunc sort , CompareFunc comp)
{
    sort((void **)p_base , m_size , comp);

    return SUCCEED;
}
