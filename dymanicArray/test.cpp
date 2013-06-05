/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/12 23:47:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  NDSL
 *
 * =====================================================================================
 */
#include "dArray.h"
#include<time.h>
#include<stdlib.h>
#include "sort.h"
#include <map>

using namespace std;

#define TIMES  10
#define TEST_TIMES 1
#define RANDOM() (rand() % (TIMES * 4))

int compCount = 0;

bool equal(void *ctx , void *data)
{
    return (*(int *)ctx == *(int *)data);
}

void print(void *ctx , void *data)
{
    printf("%d " , *(int *)data);
}

bool bigger(void *first , void *second)
{
    ++ compCount;
    return (*(int *)first > *(int *)second);
}

void usage()
{
    cerr<<"0 : select sort..."<<endl;
    cerr<<"1 : bubble sort..."<<endl;
    cerr<<"2 : insert sort..."<<endl;
    cerr<<"3 : shell  sort..."<<endl;
    cerr<<"4 : heap   sort..."<<endl;
    cerr<<"5 : quick  sort..."<<endl;
}

void init_map(map<int , SortFunc> &mp)
{
    mp.insert(make_pair(SELECT , selectSort));
    mp.insert(make_pair(BUBBLE , bubbleSort));
    mp.insert(make_pair(INSERT , insertSort));
    mp.insert(make_pair(SHELL , shellSort));
    mp.insert(make_pair(HEAP , heapSort));
    mp.insert(make_pair(QUICK , quickSort));
}

int main(int argc , char *argv[])
{
//    srand(time(NULL));
    
    if(argc != 2)
    {
        printf("argc error !\n");
        return -1;
    }

    usage();

    map<int , SortFunc> func_map;
    init_map(func_map);

    int k = 0;
    for( k = 0 ; k < TEST_TIMES ; ++ k)
    {
    DArray *darray = new DArray;
    int i = 0;
    for( ; i < TIMES ; ++ i)
    {
        if(i % 3 == 0)
            darray->dPrepend(RANDOM());
        else if(i % 3 == 1)
            darray->dAppend(RANDOM());
        else
            darray->dInsert(RANDOM() , RANDOM());
    }

    printf("First element : %d\n" , darray->dGetByIndex(0));
  
    {
        darray->dForeach(print , NULL);
        printf("\n");
        printf("array size is %d , length is %d\n" , (int)(darray->dGetSize()) 
                , (int)(darray->dGetLength()));
        int value = 16;
        int first = darray->dFind(equal , &value);
        printf("16 appear first is %d\n" , first);
    }

    int choice = atoi(argv[1]);
    if(choice >= MAX_CHOICE)
    {
        printf("Invalid choice !\n");
        return -1;
    }

    darray->dSort(func_map[choice] , bigger);
    darray->dForeach(print , NULL);
    printf("\n");

/*  
    for(i = 0 ; i < TIMES ; ++ i)
    {
        int value = 0;
        if(i % 5 == 1)
            darray->dDelete(3);
        if(i % 5 == 3)
            darray->dDelete(3 , &value);
    }

    {
        darray->dForeach(print , NULL);
        printf("\n");
        printf("array size is %d , length is %d\n" , darray->dGetSize() , darray->dGetLength());
        int value = 16;
        size_t first = darray->dFind(equal , &value);
        printf("16 appear first is %d\n" , first);
    }
*/
    delete darray;
    }
    printf("avarage compare count is %d\n" , compCount / TEST_TIMES);
    return 0;
}

