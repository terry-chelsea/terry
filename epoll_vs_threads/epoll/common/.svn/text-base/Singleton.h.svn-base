#ifndef _SINGLETON_H
#define _SINGLETON_H
#include <cstddef>

//it can make sure there is only one instance in one node

template <class T>
class Singleton
{
private:
    Singleton(const Singleton&) {}
    Singleton& operator = (const Singleton&) {}
    static T * m_pInstance;
protected://modify by fb 0323
    Singleton() {}
    virtual ~Singleton() {}
public:
    static T* getInstancePtr();
    static T& instance()
    {
        //static T theInstance;
        if(m_pInstance == NULL)
        {
            m_pInstance = new T;
        }
        return *m_pInstance;
    }
};

template <class T>
T* Singleton<T>::m_pInstance = NULL;
//get the single instance of the given class
//template<class T> T* Singleton<T>::m_pInstance = NULL;
template<class T> T* Singleton<T>::getInstancePtr(void)
{
    //static T* instance =NULL;
    if(m_pInstance == NULL)
    {
        m_pInstance = new T;
    }
    return m_pInstance;
}
#endif
