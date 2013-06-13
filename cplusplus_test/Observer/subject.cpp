/*
 * =====================================================================================
 *
 *       Filename:  subject.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/13 22:10:50
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

Subject::Subject(string name)
    :m_name(name) ,
    m_laugh(NULL) , 
    m_cry(NULL)
{

}


Subject::~Subject()
{
    cerr<<m_name<<" 被析构了，呜呜~~~"<<endl;

    this->quit_laugh();
    
    this->quit_cry();
}

void Subject::join_laugh(Observer *lau)
{
    if(NULL == lau)
        return ;

    cerr<<m_name<<" 加入了 "<<lau->get_name()<<endl;

    lau->Register(this);
    m_laugh = lau;
}

void Subject::join_cry(Observer *cry)
{
    if(NULL == cry)
        return ;

    cerr<<m_name<<" 加入了 "<<cry->get_name()<<endl;

    cry->Register(this);
    m_cry = cry;
}

void Subject::quit_laugh()
{
    if(m_laugh != NULL)
    {
        cerr<<m_name<<" 退出了 "<<m_laugh->get_name()<<endl;
        m_laugh->unregister(this);
        m_laugh = NULL;
    }
}

void Subject::quit_cry()
{
    if(m_cry != NULL)
    {
        cerr<<m_name<<" 退出了 "<<m_laugh->get_name()<<endl;
        m_cry->unregister(this);

        m_cry = NULL;
    }
}

void Subject::start_laugh()
{
    if(m_laugh != NULL)
        m_laugh->notifyAll(this);
}

void Subject::start_cry()
{
    if(m_cry != NULL)
        m_cry->notifyAll(this);
}

void Subject::laugh(string name)
{
    cerr<<"我是 "<<m_name<<" 听到了 "<<name<<" 的笑声..."<<endl;
}

void Subject::cry(string name)
{
    cerr<<"我是 "<<m_name<<" 听到了 "<<name<<" 的哭声..."<<endl;
}

void Subject::stop_cry()
{
    cerr<<m_cry->get_name()<<" 注销了，我不能再看别人哭了...by "<<m_name<<endl;
    m_cry = NULL;
}

void Subject::stop_laugh()
{
    cerr<<m_laugh->get_name()<<" 注销了，我不能再看别人哭了...by "<<m_name<<endl;
    m_laugh = NULL;
}
