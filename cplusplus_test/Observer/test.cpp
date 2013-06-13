/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/13 22:34:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */


#include "subject.h"
#include "observer.h"

#include <iostream>

using namespace std;

int main()
{
    Observer *laugh = new LaughObserver();
    Subject sub1("冯宇");
    Subject sub2("冯标");

    sub1.join_laugh(laugh);
    sub2.join_laugh(laugh);

    Observer *cry = new CryObserver();
    Subject sub3("冯豪");
    Subject sub4("冯源");

    sub3.join_cry(cry);
    sub4.join_cry(cry);

    sub3.join_laugh(laugh);
    sub4.join_laugh(laugh);


    sub1.start_laugh();
    sub4.start_cry();

    sub2.start_cry();

    delete cry;

    sub1.quit_laugh();

    sub3.quit_laugh();
    sub4.quit_laugh();

    delete laugh;

    return 0;
}
