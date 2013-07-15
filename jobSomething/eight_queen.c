#include <stdio.h>

#define QUEENS  8
static int all_solutions = 0;

int check(int *arrs , int num)
{
    int i = 0 , j = 0;
    for(i = 0 ; i < num ; ++ i)
    {
        for(j = i + 1 ; j < num ; ++ j)
        {
            if(j - i == arrs[j] - arrs[i] || 
                    i - j == arrs[j] - arrs[i])
                return 0;
        }
    }

    return 1;
}

void print_solution(int *arrs , int num)
{
    printf("Solution %d : " , all_solutions);

    int i = 0;
    for(i = 0 ; i < num ; ++ i)
    {
        printf("%2d" , arrs[i]);
    }

    printf("\n");
}

void solution(int *arrs , int num , int cur)
{
#if 1
    if(cur == num)
    {
        if(check(arrs , num))
        {
            ++ all_solutions;
//            print_solution(arrs , num);
        }
    }
#else 
    if(!check(arrs , cur + 1))
    {
        return ;
    }
    else if(cur == num)
    {
        ++ all_solutions;
        print_solution(arrs , num);
    }
#endif
    else 
    {
        int i = 0;
        for(i = 0 ; i < num ; ++ i)
        {
            int tmp = arrs[i];
            arrs[i] = arrs[cur];
            arrs[cur] = tmp;
            solution(arrs , num , cur + 1);
            tmp = arrs[cur];
            arrs[cur] = arrs[i];
            arrs[i] = tmp;
        }
    }
}

void eight_queen()
{
    int arrs[QUEENS];
    int i = 0 ; 
    for(i = 0 ; i < QUEENS ; ++ i)
        arrs[i] = i;

    solution(arrs , QUEENS , 0);
}


int main()
{
    eight_queen();
    printf("All solutions : %d\n" , all_solutions);
}
