#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 4096

int deal_with_input(char *line , int *res , int num)
{
    char *cur = line;
    char *end = line + strlen(line) - 1;

    int index = 0;
    char *flag = cur;
    while(cur != end)
    {
        if(',' == *cur)
        {
            cur[0] = '\0';
            res[index ++] = atoi(flag);
            flag = cur + 1;
            cur[0] = ',';
        }
        
        if(index == num)
            break;
        cur ++;
    }
    if(cur == end)
    {
        res[index ++] = atoi(flag);
    }

    return index;
}

void func(int *start , int *end , int sum , int cur_sum , int *all)
{
    if(start >= end || *start + cur_sum >= sum)
    {
        if(*start + cur_sum == sum)
            (*all) ++;
    }
    else 
    {
        func(start + 1 , end , sum , cur_sum + *start , all);
        func(start + 1 , end , sum , cur_sum , all);
    }
}

void sort(int *array , int n)
{
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
                int temp = array[i];
                for(j = i ; j > k; j -= begin)
                {
                    if(temp > array[j - begin])
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

int calculate(int *array , int num , int sum)
{
    int all = 0;

    sort(array , num);

    func(array , array + num - 1 , sum , 0 , &all);

    return all;
}

#define NUMBER_LEN 8

int main()
{
    int test_nums = 0;
    char line[MAX_LINE];
    char number_char[NUMBER_LEN];
    memset(number_char , 0 , NUMBER_LEN);

    if(fgets(number_char , NUMBER_LEN - 1 , stdin) == NULL)
        return -1;

    test_nums = atoi(number_char);
    if(test_nums <= 0)
        return -1;

    int *result = (int *)malloc(sizeof(int) * test_nums);
    memset(result , 0 , sizeof(int) * test_nums);

    int i = 0;
    for(i = 0 ; i < test_nums ; ++ i)
    {
        memset(line , 0 , MAX_LINE);
        fgets(number_char , NUMBER_LEN - 1 , stdin);
        int numbers = atoi(number_char);
        fgets(number_char , NUMBER_LEN - 1 , stdin);
        int sum = atoi(number_char);

        if(numbers <=0)
            goto RET;
        
        if(fgets(line , MAX_LINE - 1 , stdin) == NULL)
            goto RET;

        int len = strlen(line);
        if('\n' == line[len - 1])
            line[len - 1] = '\0';

        int *input_numbers = (int *)malloc(sizeof(int) * numbers);
        memset(input_numbers , 0 , sizeof(int) * numbers);

        numbers = deal_with_input(line , input_numbers , numbers);
        
        result[i] = calculate(input_numbers , numbers , sum);

        free(input_numbers);
        input_numbers = NULL;
    }


    for(i = 0 ; i < test_nums ; ++ i)
    {
        printf("%d\n" , result[i]);
    }

RET :
    free(result);
    result = NULL;
    
    return 0;
}
