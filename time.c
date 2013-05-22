#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//正确的时间格式 : 年-月-日 或者 年/月/日 ，并且年月日都是数字
//并且年占用4字节，月占用2字节，日占用2字节，一共10个字节
//其他的任何格式都是错误格式，返回错误信息

typedef struct 
{
    int year;
    int month;
    int day;
}Date;

typedef struct list 
{
    int  result;
    struct list *next;
}Item;


typedef struct 
{
    Item *head;
    Item *rear;
}LIST;


#define MAX_LINE 32
#define START_YEAR 1900
#define START_MONTH 1
#define START_DAY  0
#define ONE_DAY (24 * 60 * 60)
#define ERROR_LINE "Error"

static Date start_time = {2004 , 3 , 1};

int date_gap(Date start , Date end)
{
    struct tm st , ed;
    memset(&st , 0 , sizeof(st));
    memset(&ed , 0 , sizeof(ed));

    st.tm_year = start.year - START_YEAR;
    st.tm_mon = start.month - START_MONTH;
    st.tm_mday = start.day - START_DAY;

    ed.tm_year = end.year - START_YEAR;
    ed.tm_mon = end.month - START_MONTH;
    ed.tm_mday = end.day - START_DAY;

    time_t t_start = mktime(&st);
    time_t t_end = mktime(&ed);

    unsigned int gap = t_end - t_start;
    if(gap < 0 || gap % ONE_DAY)
        return -1;

    return gap / ONE_DAY;
}

#define  IS_DIGTAL(ch)  ((ch) >= '0' && (ch) <= '9')

int string_to_int(char *st , int len)
{
    int sum = 0;
    int i = 0;
    for(i = 0 ; i < len ; ++ i)
    {
        if(!IS_DIGTAL(st[i]))
            return -1;

        sum = sum * 10 + st[i] - '0';
    }

    return sum;
}

#define SEP1  '-'
#define SEP2  '/'
#define DEFAULT_DATE  "xxxx-xx-xx"
#define RUN_NIAN(year) (((year) / 4 == 0 && (year / 100) != 0) || \
        ((year / 400 == 0)))

static int days[] = {31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31};
static int run_days[] = {31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31};

int deal_with_time(char *str)
{
    Date date = {0 , 0 , 0};
    
    if(str[4] != str[7]) 
    {
        return -1;
    }

    if(str[4] != SEP1 && str[4] != SEP2)
    {
        return -1;
    }

    date.year = string_to_int(str , 4);
    date.month = string_to_int(str + 5 , 2);
    date.day = string_to_int(str + 8 , 2);

    if(date.month <= 0 || date.month > 12)
        return -1;
    if(RUN_NIAN(date.year))
    {
        if(date.day <= 0 || date.day > run_days[date.month - 1])
            return -1;
    }
    else 
    {
        if(date.day <= 0 || date.day > days[date.month - 1])
            return -1;
    }

    return date_gap(start_time , date);
}

#define INIT_LIST(list) do{ \
        list = (LIST *)malloc(sizeof(LIST)); \
        list->head = NULL; \
        list->rear = NULL; \
}while(0)

#define LIST_ADD(list , value) do{ \
    Item *it = (Item *)malloc(sizeof(Item)); \
    it->result = value; \
    it->next = NULL; \
    if(NULL == list->head && NULL == list->rear) {\
        list->head = it; \
        list->rear = it; \
    }\
    else{ \
        list->rear->next = it; \
        list->rear = it; \
    } \
}while(0)

#define DESTORY_LIST(list) do{ \
    if(list->head != list->rear) { \
        Item *it = list->head; \
        while(it != NULL) { \
            Item *tmp = it; \
            it = it->next; \
            free(tmp); \
        }\
    }\
    free(list); \
}while(0)

int main()
{
    char input[MAX_LINE];
    LIST *list = NULL;
    INIT_LIST(list);

    while(1)
    {
        memset(input , 0 , MAX_LINE);
        if(fgets(input , MAX_LINE - 1 , stdin) == NULL)
            break;

        int len = strlen(input);
        if(input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
            len -- ;
        }

        int value = 0;
        if(len != sizeof(DEFAULT_DATE) - 1)
            value = -1;
        else 
            value = deal_with_time(input);

        LIST_ADD(list , value);
    }

    Item *it = list->head;
    while(it != NULL)
    {
        int ret = it->result;
        if(-1 == ret)
            printf("%s\n" , ERROR_LINE);
        else 
            printf("%d\n" , ret);

        it = it->next;
    }

    DESTORY_LIST(list);

    return -1;
}
