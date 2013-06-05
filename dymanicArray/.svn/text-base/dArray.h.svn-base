/*
 * =====================================================================================
 *
 *       Filename:  dArray.h
 *
 *    Description:  dynamic array 
 *
 *        Version:  1.0
 *        Created:  07/05/12 21:49:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com),yu 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include<iostream>
#include<string.h>

//#define DEBUG
#define _DO_NOT_SHRINK_

#ifdef DEBUG  
#define DEFAULT_LENGTH    256
#define MIN_EXPAND_LENGTH 256
#define MIN_SPACE         128

#else

#define DEFAULT_LENGTH    16
#define MIN_EXPAND_LENGTH 8
#define MIN_SPACE         4
#endif

typedef int tagElemType;
typedef bool (*CompareFunc)(void * , void *);
typedef void (*VisitFunc)(void * , void *);
typedef void (*SortFunc)(void * , size_t , CompareFunc);

enum ret{ FAILED = 0 , SUCCEED };
typedef enum ret Ret;
class DArray
{
    public:
        DArray(size_t size = DEFAULT_LENGTH);
        DArray(tagElemType * , size_t , size_t);
        ~DArray();
        Ret dInsert(size_t , const tagElemType &);
        Ret dPrepend(const tagElemType &);
        Ret dAppend(const tagElemType &);
        Ret dDelete(size_t);
        Ret dDelete(size_t , tagElemType *);
        tagElemType & dGetByIndex(size_t);
        Ret dSetByIndex(size_t , const tagElemType &);
        size_t dGetSize();
        size_t dGetLength();
        Ret dClearAndResize(int );
//        int dFind(CompareFunc , void *);
        size_t dFind(CompareFunc , void *);
        Ret dSort(SortFunc , CompareFunc);
        Ret dForeach(VisitFunc , void *);

    private:
        bool needExpand();
        bool needShrink();
        size_t getSpaceSize();
        Ret dExpand();
        Ret dShrink();

    private:
        tagElemType *p_base;
        size_t       m_length;
        size_t       m_size;

};

#endif

