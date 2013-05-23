
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int *get_input_array(char *line , int *cnt)
{
    char *cur = line;
    while(*cur == ' ') 
        ++ cur;
    char *start = cur;

    int index = 0;
    int *array = (int *)malloc(sizeof(int) * MAX_LINE);
    memset(array , 0 , sizeof(int) * MAX_LINE);

    for( ; *cur != '\0' ; ++ cur)
    {
        if(*cur == ' ')
        {
            char *next = cur;
            while(*cur == ' ')
                ++ cur;
            if(*cur == '\0')
                break;
            cur --;

            *next = '\0';
            array[index ++] = atoi(start);
            start = cur + 1;
            *next = ' ';
        }
    }
    if(start != '\0')
    {
        array[index ++] = atoi(start);
    }

    *cnt = index;

    return array;
}

int find_max_subarray(int *array , int cnt , int *max)
{
    if(array == NULL || cnt <= 0)
        return 0;

    int sum = *max = 0;
    int i = 0;
    for(i = 0 ; i < cnt ; ++ i)
    {
        sum += array[i];
        if(sum < 0)
        {
            sum = 0;
        }
        if(sum > *max)
            *max = sum;
    }

    if(*max == 0)
    {
        *max = array[0];
        for(i = 0 ; i < cnt ; ++ i)
        {
            if(*max < array[i])
                *max = array[i];
        }
    }

    return 1;
}

int main()
{
    char line[MAX_LINE];
    while(1)
    {
        if(fgets(line , MAX_LINE - 1 , stdin) == NULL)
            break;

        int len = strlen(line);
        if(line[len - 1] = '\n')
            line[len - 1] = '\0';

        if(len <= 1)
            break;
        int count = 0;
        int *array = get_input_array(line , &count);

        int max = 0;
        if(find_max_subarray(array , count , &max) < 0)
        {
            printf("Calculate error ...\n");
        }
        else 
        {
            printf("%s max sub array sum up %d\n" , line , max);
        }

        if(array != NULL)
        {
            free(array);
            array = NULL;
        }
    }

    printf("DONE !!!\n");
    return 0;
}
