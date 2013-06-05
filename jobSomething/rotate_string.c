
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAXLEN 1024
#define LENS 20

char digtals[] = "0123456789";

void get_random_string(char *str , int len)
{
    srand(time(NULL));
    int i = 0;
    for(i = 0 ; i < len ; ++ i)
        str[i] = digtals[rand() % 10];
}


void rotate(char *start , int len , int mid);
int main()
{
    char strings[MAXLEN];
    memset(strings , '\0' , MAXLEN);

    get_random_string(strings , LENS );
    printf("Rotate To right : \n");
    int len = 0;
    scanf("%d" , &len);
    printf("string %s rotate right %d times\n" , strings , len);

    rotate(strings , LENS , len);
    printf("After rotate string : %s\n" , strings);

    return 0;
}

//#define _USE_SWAP_

#ifdef _USE_SWAP_

void swap(char *start , int from , int to)
{
    while(from < to)
    {
        int temp = start[from];
        start[from] = start[to];
        start[to] = temp;
        from ++ ;
        to --;
    }
}

void rotate(char *start , int len , int mid)
{
    swap(start , 0 , mid - 1);
    swap(start , mid , len - 1);
    swap(start , 0 , len - 1);
}

#else

int gcd(int m , int n)
{
    while(m % n)
    {
        int r = n;
        n = m % n;
        m = r;
    }

    return n;
}

void rotate(char *start , int len , int mid)
{
    int subs = gcd(len , mid);
    int times = len / subs;
    
    int i = 0;
    for(i = 0 ; i < subs ; ++ i)
    {
        char temp = start[i];
        int j = 0;
        int next = i;
        for(j = 0 ; j < times - 1 ; ++ j , next += mid)
        {
            start[next % len] = start[(next + mid) % len];
        }
        start[next % len] = temp;
    }
}

#endif
