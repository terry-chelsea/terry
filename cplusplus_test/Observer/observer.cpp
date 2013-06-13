/*
 * =====================================================================================
 *
 *       Filename:  oberver.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/13 21:47:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include "observer.h"
#include "subject.h"


void Observer::Register(Subject *sub)
{
    m_all_subs.push_back(sub);
}

void Observer::unregister(Subject *sub)
{
    m_all_subs.remove(sub);
}

LaughObserver::LaughObserver()
    :Observer()
{
    m_name = string("欢笑合唱团");

    cerr<<"我们创建了一个 "<<m_name<<" ， 欢迎注册..."<<endl;
}

LaughObserver::~LaughObserver()
{
    cerr<<m_name<<" 要注销了，I am sorry..."<<endl;

    list<Subject *>::iterator it = m_all_subs.begin();
    for( ; it != m_all_subs.end() ; ++ it)
    {
        Subject *tmp = *it;
        tmp->stop_laugh();
    }
}

void LaughObserver::notifyAll(Subject *sub)
{
    string name = sub->get_name();
    list<Subject *>::iterator it = m_all_subs.begin();
    for(; it != m_all_subs.end() ; ++ it)
    {
        Subject *tmp = *it;
        tmp->laugh(name);
    }
}

CryObserver::CryObserver()
    :Observer()
{
    m_name = string("悲情合奏队");

    cerr<<"我们创建了一个 "<<m_name<<" , 欢迎注册..."<<endl;
}

CryObserver::~CryObserver()
{
    cerr<<m_name<<" 要注销了 , I am sorry..."<<endl;

    list<Subject *>::iterator it = m_all_subs.begin();
    for( ; it != m_all_subs.end() ; ++ it)
    {
        Subject *tmp = *it;
        tmp->stop_cry();
    }
}

void CryObserver::notifyAll(Subject *sub)
{
    string name = sub->get_name();
    list<Subject *>::iterator it = m_all_subs.begin();
    for( ; it != m_all_subs.end() ; ++ it)
    {
        Subject *tmp = *it;
        tmp->cry(name);
    }
}
