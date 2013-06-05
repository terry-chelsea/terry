/*
 * =====================================================================================
 *
 *       Filename:  Singleton.cpp
 *
 *    Description:  simple test design pattern of singleton...
 *
 *        Version:  1.0
 *        Created:  06/05/13 22:39:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <iostream>
#include <pthread.h>

using namespace std;

class Singleton
{
    public : 
        //maybe can not use singleton pattern with class has parameter 
        //in his construction function...
        static Singleton *getInstance();

        void show_who_i_am();

        ~Singleton();
        Singleton(const Singleton &ls);

    private :
        Singleton();
        static Singleton *instance;
        static pthread_mutex_t mutex;
};

Singleton *Singleton::instance = NULL;
pthread_mutex_t Singleton::mutex = PTHREAD_MUTEX_INITIALIZER;

Singleton::Singleton()
{
    cout<<"I am a singleton object , you can create me only once..."<<endl;
}

Singleton::Singleton(const Singleton &ls)
{
    cout<<"You can not do this ..."<<endl;
}

Singleton::~Singleton()
{
    cout<<"I am sorry to say : I was deleted , even I do not want to.."<<endl;
}

Singleton *Singleton::getInstance()
{
    if(NULL == instance)
    {
        if(pthread_mutex_lock(&mutex) < 0)
        {
            perror("what happened , lock failed : ");
            return NULL;
        }

        if(NULL == instance)
        {
            instance = new Singleton();
        }
        pthread_mutex_unlock(&mutex);
    }

    return instance;
}

void Singleton::show_who_i_am()
{
    cout<<"Look ! This is where I am : "<<instance<<endl;
}


int main()
{
    int i = 0;
    for(i = 0 ; i < 10 ; ++ i)
    {
        Singleton *handle = Singleton::getInstance();
        handle->show_who_i_am();
    }

    //just test...
//    Singleton *here = new Singleton();
    Singleton there(*Singleton::getInstance());
    there.show_who_i_am();

    delete Singleton::getInstance();

    return 0;
}
