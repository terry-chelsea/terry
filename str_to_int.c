
/* 实现一段字符串转换为应的int类型的函数
 * 如果超出int所能够表示的范围返回MAX_INT或者MIN_INT
 * 考虑正负号，两端的空格可以去除
 * 对于"+"或者"-"符号，必须与数字相连
 * 如果字符串中包含非数字之外的字符，返回之前处理的数字的大小
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define IS_DIGTAL(ch)  (((ch) >= '0') && ((ch) <= '9'))

#define MAX_INT_VALUE    0x7FFFFFFF
#define MIN_INT_VALUE    0x80000000

char *string_strip(char *str , char sep)
{
    int len = strlen(str);
    char *last = str + len - 1;
    char *head = str;

    while(*head == sep)
        head ++;
    while(*last == sep)
        last --;

    *(last + 1) = '\0';
    return head;
}

unsigned int to_int(char *str) 
{
    unsigned int sum = 0;
    while(*str != '\0')
    {
        char ch = str[0];
        if(!IS_DIGTAL(ch))
            break;
       
        int value = ch - '0';
        sum = sum * 10 + value;
        if(sum > MAX_INT_VALUE)
            break;

        ++ str;
    }

    return sum;
}

int str_to_int(char *data)
{
    char *str = string_strip(data , ' ');
   
    unsigned int number = 0;
    value = 0;
    if(IS_DIGTAL(str[0]))
    {
        number = to_int(str);
        if(number == 0)
            value = 0;
        else if(number > MAX_INT_VALUE)
            value = MAX_INT_VALUE;
        else 
            value = number;
    }
    else if('-' == str[0])
    {
        number = to_int(str + 1);
        if(0 == number)
            value = 0;
        else if(number - 1 >= MAX_INT_VALUE)
            value = MIN_INT_VALUE;
        else 
            value = (-1) * number;
    }
    else if('+' == str[0])
    {
        number = to_int(str + 1);
        if(number == 0)
            value = 0;
        else if(number > MAX_INT_VALUE)
            value = MAX_INT_VALUE;
        else 
            value = number;
    }
    else 
        value = 0;

    return value;
}

int main()
{
    char buf[128];
    while(1)
    {
        if(fgets(buf , 128 , stdin) == NULL)
            break;

        int len = strlen(buf);
        if(buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        int ret = str_to_int(buf);
        printf("%s ====> value : %d\n" , buf , ret);
    }

    return 0;
}
