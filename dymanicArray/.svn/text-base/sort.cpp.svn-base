/*
 * =====================================================================================
 *
 *       Filename:  sort.cpp
 *
 *    Description:  sort functions that used for sorting array...
 *
 *        Version:  1.0
 *        Created:  07/06/12 01:10:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (fengyuatad@126.com), yu
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "sort.h"
#define DEBUG

static tagElemType *selectMostOne(tagElemType *array , size_t n , CompareFunc comp)
{
    unsigned int i = 0;
    tagElemType *max = array;

    for(i = 0 ; i < n ; ++ i)
    {
        if(comp(max , array + i))
            max = array + i;
    }

    return max;
}

static void swap(tagElemType *first , tagElemType *second)
{
    tagElemType temp;
    if(first != second)
    {

        temp = *first;
        *first = *second;
        *second = temp;
    }
}

void selectSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;
    size_t i = 0;
    for(i = 0 ; i < n ; ++i)
    {
        tagElemType *max = selectMostOne(array + i , n - i , comp);
        swap(max , array + i);
    }
}

void bubbleSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;
    size_t i = 0;
    size_t j = 0;
    bool flag = true;

    for(i = 0 ; i < n - 1 ; ++i)
    {
        if(false == flag)
            break;

        flag = false;
        for(j = 0 ; j < n - 1 - i ; ++ j)
        {
            if(comp(&array[j] , &array[j + 1]))
            {
                swap(array + j , array + j + 1);
                flag = true;
            }

        }
    }
}

void insertSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;
    size_t i = 0;
    size_t j = 0;

    for(i = 0 ; i < n - 1 ; ++i)
    {
        tagElemType temp = array[i + 1];
        for(j = i + 1 ; j > 0 ; --j)
        {
            if(comp(&temp , &array[j - 1]))
                break;
            else
                array[j] = array[j - 1];
        }
        array[j] = temp;
    }
}

void shellSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;
    size_t begin = n / 2;
    while(begin > 0)
    {
        size_t i = 0;
        size_t k = 0;
        size_t j = 0;
        for(k = 0 ; k < begin ; ++k)
        {
            for(i = k + begin ; i < n ; i += begin)
            {
                tagElemType temp = array[i];
                for(j = i ; j > k; j -= begin)
                {
                    if(comp(&temp , &array[j - begin]))
                        break;
                    else
                        array[j] = array[j - begin];
                }
                array[j] = temp;
            }
        }
        begin = begin / 2;
    }
}

static void buildHeap(tagElemType *array , size_t n , CompareFunc comp)
{
    size_t i = 0;
    for(i = n - 1 ; i > 0 ; -- i)
    {
        if(comp(&array[i] , &array[(i - 1) >> 1]))
        {
            swap(&array[i] , &array[(i - 1) >> 1]);
            size_t k = i;
            while((k << 1) + 1 < n)
            {
                size_t j = (k << 1) + 1;
                if(comp(&array[j + 1] , &array[j]))
                {
                    j = j + 1;
                }
                if(comp(&array[j] , &array[k]))
                {
                    swap(&array[j] , &array[k]);
                    k = j;
                    continue ; 
                }
                else
                    break;
            }
        }
    }
}

static void adjustHeap(tagElemType *array , size_t n , CompareFunc comp)
{
    size_t k = 0;
    size_t i = 0;
    while((k << 1) + 1 < n)
    {
        if(((k << 1) + 2) >= n)
            i = ((k << 1) + 1);
        else
        {
            i = ((k << 1) + 2);
            if(comp(&array[i - 1] , &array[i]))
            {
                i = i - 1;
            }
        }

        if(comp(&array[i] , &array[k]))
        {
            swap(&array[i] , &array[k]);
            k = i;
        }
        else
            break;

    }
}

#ifdef DEBUG
static void print(tagElemType *array , size_t n)
{
    size_t i = 0;
    for(i = 0 ; i < n ; ++ i)
        printf("%d " , array[i]);

    printf("\n");
}
#else
static void print(tagElemType *array , size_t n)
{

}
#endif

void heapSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;

    buildHeap(array , n , comp);
    print(array , n);
    size_t i = 0;
    for(i = 0 ; i < n - 1; ++ i)
    {
        swap(&array[0] , &array[n - i - 1]);
        adjustHeap(array , n - i - 1 , comp);
        print(array , n);
    }
}

static void doQuickSort(tagElemType *array , size_t low , size_t high , CompareFunc comp , size_t n)
{
    if(low >= high)
        return ;

    size_t keyHighIndex = high;
    size_t keyLowIndex = low;
    size_t keyIndex = low;
    tagElemType key = array[keyIndex];

    while(keyHighIndex > keyLowIndex)
    {
        while(!comp(&key , &array[keyHighIndex]) && keyHighIndex > keyLowIndex)
        {
            -- keyHighIndex;
        }
        swap(&array[keyHighIndex] , &array[keyLowIndex]);
        keyIndex = keyHighIndex;
        ++ keyLowIndex;

        if(keyLowIndex >= keyHighIndex)
            break;

        while(!comp(&array[keyLowIndex] , &key) && keyLowIndex < keyHighIndex)
        {
            ++ keyLowIndex;
        }
        swap(&array[keyHighIndex] , &array[keyLowIndex]);
        keyIndex = keyLowIndex;
        -- keyHighIndex;
    }
    
    print(array , n);

    if(keyIndex > low + 1)
        doQuickSort(array , low , keyIndex - 1 , comp , n);
    if(high > keyIndex + 1)
        doQuickSort(array , keyIndex + 1, high , comp , n);
}

void quickSort(void *pArray , size_t n , CompareFunc comp)
{
    if(n < 2 || NULL == pArray)
        return ;

    tagElemType *array = (tagElemType *)pArray;

    doQuickSort(array , 0 , n - 1 , comp , n);
}
