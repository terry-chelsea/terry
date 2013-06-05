/*
 * =====================================================================================
 *
 *       Filename:  sort.h
 *
 *    Description:  interface of sort function...
 *
 *        Version:  1.0
 *        Created:  07/06/12 01:25:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com), yu
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#ifndef _SORT_H_
#define _SORT_H_

#include<iostream>

typedef bool (*CompareFunc)(void * , void *);

typedef int  tagElemType; 

void selectSort(void *pArray , size_t n , CompareFunc comp);

void bubbleSort(void *pArray , size_t n , CompareFunc comp);

void insertSort(void *pArray , size_t n , CompareFunc comp);

void shellSort(void *pArray , size_t n , CompareFunc comp);

void heapSort(void *pArray , size_t n , CompareFunc comp);

void quickSort(void *pArray , size_t n , CompareFunc comp);

typedef enum
{
    SELECT = 0 ,
    BUBBLE , 
    INSERT , 
    SHELL , 
    HEAP , 
    QUICK , 
    MAX_CHOICE
}SORT_TYPE;

#endif
