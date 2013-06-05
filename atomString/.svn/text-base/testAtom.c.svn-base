/*
 * =====================================================================================
 *
 *       Filename:  testAtom.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/04/12 15:01:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "atom.h"
#include<time.h>
#include<stdlib.h>
#include<sys/time.h>

#define ADD_NUMBER       200000
#define STRING_NUMBER    100000
#define INT_NUMBER       100000
#define LENGTH_NUMBER    100000
#define FREE_NUMBER      100000
#define MAX_TEST_NUMBER      (~(1 << (8 * sizeof(int) - 1)))
#define MAX_CHAR_LENGTH  100
#define DEFAULT_CHAR_LENGTH  16
#define INIT_NUMBER      2039

static char *gArray[ADD_NUMBER];
const  char characters[] = "_abcdefghijklmnopqrstuvwxyz0123456789";
const  int  TABLE_SIZE = sizeof(characters) / sizeof(characters[0]) - 1;

char *getRandomLine(int *len);
void makeRandomArray(char *array , int length);

int main()
{
    struct timeval start , end;
    double counter = 0;
    double allCounter = 0;
    gettimeofday(&start , NULL);
    srand(time(NULL));
    atomInit(INIT_NUMBER);    
    int i = 0 ;
    
    int size = -1;
    for(i = 0 ; i < ADD_NUMBER ; ++ i)
    {
        gArray[i] = getRandomLine(&size);
        atomAdd(gArray[i] , size);
    }
 
    printf("\n--------------------after add-----------------------\n");
//    atomShow();

    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    int intNumber = -1;
    for(i = 0 ; i < INT_NUMBER ; ++ i)
    {
        intNumber = rand() % MAX_TEST_NUMBER;
        atomInt(intNumber);
    }

    printf("\n--------------------after int-----------------------\n");
//    atomShow();
    
    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    char testArray[MAX_CHAR_LENGTH + 1];
    int arraySize = -1;
    for(i = 0 ; i < STRING_NUMBER ; ++ i)
    {
        arraySize = rand() % MAX_CHAR_LENGTH;
        makeRandomArray(testArray , arraySize);
        testArray[arraySize] = '\0';
        atomString(testArray);
    }

    printf("\n---------------------after string----------------------\n");
//    atomShow();

    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    int testIndex = -1;
    for(i = 0 ; i < LENGTH_NUMBER ; ++ i)
    {
        testIndex = rand() % ADD_NUMBER;
        if(NULL == gArray[testIndex])
            continue;
        atomLength(gArray[testIndex]);
        
        gArray[testIndex] = NULL;
    }

    printf("\n---------------------after length----------------------\n");
//    atomShow();

    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    for(i = ADD_NUMBER - 1 ; i >= 0 ; -- i)
    {
        if(NULL == gArray[i])
            continue;
        
        atomFree(gArray[i]);
    }

    printf("\n---------------------after free----------------------\n");
//    atomShow();

    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    atomReset();
    printf("\n----------------------after reset---------------------\n");
//    atomShow();
    gettimeofday(&end , NULL);
    counter = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;
    printf("cost time : %.6lf\n" , counter / 1000000);
    gettimeofday(&start , NULL);
    allCounter += counter;

    printf("\n----------------------all used time---------------------\n");
    printf("All used time is :  %.6lf\n" , allCounter / 1000000);
    return 0;
}

void getLine(char *ptr , int length)
{
    ptr[0] = characters[rand() % 27];

    int i = 0 ;
    for(i = 1 ; i < length ; ++ i)
        ptr[i] = characters[rand() % TABLE_SIZE];
}

char *getRandomLine(int *len)
{
    int random = -1;
    int length = -1;
    
    length = rand() % MAX_CHAR_LENGTH;
    if(0 == length)
        length = DEFAULT_CHAR_LENGTH;

    char *temp = new char[length + 1];
    assert(temp != NULL);
    *len = length;

    getLine(temp , length);

    return temp;
}

void makeRandomArray(char *array , int length)
{
    if(0 == length)
        length = DEFAULT_CHAR_LENGTH;
    getLine(array , length);    
}
