/*
 * =====================================================================================
 *
 *       Filename:  oberver.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/13 21:36:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <list>
#include <string>
#include <iostream>

using namespace std;

class Subject;
//抽象基类，仅提供接口
class Observer 
{
    public :
        Observer()
            :m_name() , 
            m_all_subs()
        {}

        virtual ~Observer()
        {}

        virtual void Register(Subject *);

        virtual void unregister(Subject *);

        virtual void notifyAll(Subject *) = 0;

        string get_name()
        {
            return m_name;
        }

    protected :
        string          m_name;
        list<Subject *> m_all_subs;
};


class LaughObserver : public Observer
{
    public : 
        LaughObserver();
        ~LaughObserver();
        void notifyAll(Subject *);
};

class CryObserver : public Observer
{
    public :
        CryObserver();
        ~CryObserver();
        void notifyAll(Subject *);
};
