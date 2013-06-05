/*
 * =====================================================================================
 *
 *       Filename:  vector.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/22/13 10:56:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Heaven (), zhanwenhan@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int arr[] = {1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9};
    vector<int> vec(arr , arr + sizeof(arr) / sizeof(int));

    vector<int>::iterator first = vec.begin();

    vector<int>::iterator fifth = first + 5;

    cerr<<"first : "<<*first<<" and fifth : "<<*fifth<<endl;

    return 0;
}
