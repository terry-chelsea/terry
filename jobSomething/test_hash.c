
#include "HashFunc.h"
#include <stdlib.h>
#include <unistd.h>

//char *line1 = "1234567890123";
//char *line2 = "3210987654321";
#define MAX  1000
int main()
{
    char str[16];
    int ret = createHashFuncObj(36);
    if(ret < 0)
    {
        printf("Create object error !");
        return -1;
    }

    unsigned int temp = 0;
    int i = 0;
    srand(getpid());
    int num1 = rand() % MAX;
    int len = snprintf(str , 16 , "%d" , num1);
    printf("test number %s len is %d\n" , str , len);

    for_each_result(str , strlen(str) + 1 , temp)
    {
        printf("NO.%d hash result : %u\n" , ++i , temp);
    }

    printf("--------------------------------------\n");

    num1 = rand() % MAX;
    snprintf(str , 16 , "%d" , num1);
    printf("test number %s\n" , str);
    for_each_result(str , strlen(str) + 1 , temp)
    {
        printf("NO.%d hash result : %u\n" , ++i , temp);
    }

    destoryHashFuncObj();
    return 0;
}
