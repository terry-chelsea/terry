//得到一个字符串的全部子字符串集合

#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 256
static int all_solutions = 0;
static char stack[MAX_LENGTH];
static int  _index = 0;

void push(char val)
{
    stack[_index] = val;
    ++ _index;
}

char pop()
{
    -- _index;
    char tmp = stack[_index];
    stack[_index] = '\0';
    return tmp;
}

void print()
{
    int tmp = stack[_index];
    stack[_index] = '\0';
    printf("%s\n" , stack);
    stack[_index] = tmp;
}

void combination(const char *str , int len)
{
    if(len == 0)
    {
        print();
        return ;
    }

    if('\0' == *str)
        return ;

    push(*str);
    combination(str + 1 , len - 1);
    pop();
    combination(str + 1 , len);
}

void string_combinations(const char *str)
{
    if(NULL == str)
    {
        printf("NULL pointer string...\n");
        return ;
    }

    int len = strlen(str);
    int i = 0;
    for(i = 1 ; i <= len ; ++ i)
    {
        combination(str , i);
    }

    return ;
}

int main()
{
    char str[MAX_LENGTH];
    while(1)
    {
        int len = 0;
        if(fgets(str , MAX_LENGTH - 1 , stdin) == NULL)
            break;

        len = strlen(str);
        if(str[len - 1] == '\n')
        {
            str[len - 1] = '\0';
            -- len;
        }

        printf("Combinations of : %s\n" , str);
        string_combinations(str);
    }
}
