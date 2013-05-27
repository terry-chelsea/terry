
//题目:在一个数组中包含多个整数，其中只有两个出线一次，其它的都出现两次，如何能够找到这两个不一样的整数，时间复杂度为O(n)，空间复杂度为O(1)，如果是三个不一样的整数呢？
//其实可以演化成数组中有两个数出现奇数次，其它的都出现偶数次

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *first , int *second)
{
    int tmp = 0;
    if(first != second)
    {
        tmp = *first;
        *first = *second;
        *second = tmp;
    }
}

//输入为一个整数说组，结果找出两个只出现一次的整数，结果保存在数组的前两个元素中
int find_2_diff_number(int *array , int len)
{
    if(NULL == array)
        return -1;

    int i = 0;
    int sum_xor = 0;
    //首先计算出所有的元素的异或值
    for(i = 0 ; i < len ; ++ i)
        sum_xor ^= array[i];

    //如果总的异或值等于0，说明数组中的每个元素都出现了偶数次，返回
    if(!sum_xor)
    {
        printf("Wrong type array !\n");
        return -1;
    }
    
    //找到第一个为1的位，说明不相同的两个数中有一个的该位为1，另外一个为0
    int bit = 0;
    while(!(sum_xor & 0x1))
    {
        ++ bit;
        sum_xor >>= 1;
    }
    bit = 1 << bit;

    int ret1 = 0 , ret2 = 0;
    for(i = 0 ; i < len ; ++ i)
    {
        if(array[i] & bit)
            ret1 ^= array[i];
        else 
            ret2 ^= array[i];
    }
    printf("Different number : %d , %d\n" , ret1 , ret2);

    return 0;
}

int find_lowest(int n)
{
    if(!n)
        return 0;

    int cnt = 0;
    while(!(n & 0x1))
    {
        cnt ++;
        n >>= 1;
    }

    return cnt;
}

//找出三个只出现一次的数字，假设所有的元素的异或等于x
//只能通过判断x^a,x^b和x^c中的最低位为1的位来判断
//假设x^a,x^b和x^c最低位为1的位分别为x,y,z，那么m=x^y^z不为0
//因为x,y,z中都是只有一个1(二进制表示)，如果他们等于0，那么
//x,y,z都必须相等！那么x^a^x^b^x^c对应的该位就为1，所以不可能
//因此m就等于x,y,z中的一个数，并且x^a,x^b和x^c中的第m为只有一个为1
//如果三个都为1，x与a,b,c在m位上都相反，那么就不可能得到x=a^b^c了
void find_3_diff_number(int *array , int len)
{
    if(NULL == array)
        return ;

    int i = 0 , sum_xor = 0;
    for(i = 0 ; i < len ; ++ i)
        sum_xor ^= array[i];

    int last_xor = 0;
    for(i = 0 ; i < len ; ++ i)
        last_xor ^= find_lowest(array[i] ^ sum_xor);

    int ano_xor = 0;
    for(i = 0 ; i < len ; ++ i)
    {
        if(last_xor == find_lowest(sum_xor ^ array[i]))
        {
            ano_xor ^= array[i];
        }
    }

    printf("find one number : %d\n" , ano_xor);
    for(i = 0 ; i < len ; ++ i)
    {
        if(array[i] == ano_xor)
        {
            array[i] = array[0];
            array[0] = ano_xor;
        }
    }

    find_2_diff_number(array + 1 , len - 1);
}

int main()
{
    int a[] = {1,3,5,7,9,100,9,2,4,6,1024,8,10,1,21,3,7,5,6,8,2,4,10};
    find_3_diff_number(a , sizeof(a) / sizeof(a[0]));
    return 0;
}
