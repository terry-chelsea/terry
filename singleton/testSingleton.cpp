/*
 * =====================================================================================
 *
 *       Filename:  testSingleton.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/12 12:57:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "singletonBase.h"
#include<iostream>

using namespace std;

class TestSingleton : public SingletonBase<TestSingleton>
{
    public:
        int getValue()
        {
            return m_value;
        }
        void setValue(int value)
        {
            m_value = value;
        }

    private:
        TestSingleton()
        {
            m_value = 0;
        }
        ~TestSingleton()
        {

        }

    private:
        friend class SingletonBase<TestSingleton>;
        int m_value;
};

int main()
{
    cout<<"Begin : "<<endl;

    TestSingleton *test = TestSingleton::getInstance();
    cout<<"address of getinstance is : "<<hex<<test<<dec<<endl;

    cout<<"get value : "<<TestSingleton::getInstance()->getValue()<<endl;

    TestSingleton::getInstance()->setValue(16);
    
    cout<<"get value : "<<TestSingleton::getInstance()->getValue()<<endl;

    return 0;
}
