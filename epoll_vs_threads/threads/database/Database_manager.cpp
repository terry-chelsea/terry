
#include "Database_manager.h"
#include "Database_handle.h"

#define LOG_INSIDE_ERROR (std::cerr)
#define LOG_INSIDE_INFO  (std::cout)
#define LOG_END          (std::endl)

DBManager::DBManager()
    :m_ppool(NULL) , 
    m_purl(NULL) , 
    m_binit(false) , 
    m_waiting_counter(0)
{
}

DBManager::~DBManager()
{
    if(m_binit)
    {
        ConnectionPool_free(&m_ppool);
        m_ppool = NULL;

        URL_free(&m_purl);
        m_purl = NULL;
        m_binit = false;
    }
}

int DBManager::initialize(const std::string & url_str , int max_conn)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr , PTHREAD_MUTEX_RECURSIVE_NP);
    if(pthread_mutex_init(&m_mutex , &attr) < 0)
    {
        LOG_INSIDE_ERROR<<"Mutex in database manager initialize error !"<<LOG_END;
        return -1;
    }

    pthread_condattr_t cond_attr;
    if(pthread_cond_init(&m_cond , &cond_attr) < 0)
    {
        LOG_INSIDE_ERROR<<"Condition in database manager initialize error !"<<LOG_END;
        pthread_mutex_destroy(&m_mutex);
        return -1;
    }

    URL_T url = URL_new(url_str.c_str());
    if(NULL == url)
    {
        LOG_INSIDE_ERROR<<"DBManager::Create new URL handle error , error URL : "
            <<url_str<<LOG_END;
        return -1;
    }

    ConnectionPool_T pool = ConnectionPool_new(url);
    if(NULL == pool)
    {
        LOG_INSIDE_ERROR<<"DBManager::Create new connection pool error !"<<LOG_END;
        URL_free(&url);
        return -1;
    }

    m_ppool = pool;
    m_purl = url;
    m_binit = true;
    
    return 0;
}

DBHandle *DBManager::new_connection()
{
    pthread_mutex_lock(&m_mutex);
    Connection_T conn = ConnectionPool_getConnection(m_ppool);
    while(conn == NULL)
    {
        m_waiting_counter ++;
        pthread_cond_wait(&m_cond , &m_mutex);
        m_waiting_counter --;
        conn = ConnectionPool_getConnection(m_ppool);
    }

    pthread_mutex_unlock(&m_mutex);

    DBHandle *handle = new DBHandle(this , conn);
    if(NULL == handle)
    {
        LOG_INSIDE_ERROR<<"DBManager::create DBHandle error !"<<LOG_END;
        ConnectionPool_returnConnection(m_ppool , conn);
        return NULL;
    }

    return handle;
}

/*  
DBHandle *DBManager::new_connection()
{
    Connection_T conn = ConnectionPool_getConnection(m_ppool);
    int cnt = 0;
    while(NULL == conn)
    {
//        LOG_INSIDE_ERROR<<"DBManager::No connection left in connection pool! you can set max connections or retry later..."<<LOG_END;
//        return NULL;

        //说明连接池中无连接可用，等待直到连接释放
        if(pthread_mutex_lock(&m_mutex) < 0 )
        {
            LOG_INSIDE_ERROR<<"Lock Mutex error in database manager !"<<LOG_END;
            return NULL;
        }
//        std::cerr<<"This is "<<++ cnt<<" time waiting for connection : "<<pthread_self()<<std::endl;
        m_waiting_counter ++;

        if(pthread_cond_wait(&m_cond , &m_mutex) < 0)
        {
            LOG_INSIDE_ERROR<<"Wait on condition error in databse manager !"<<LOG_END;
            return NULL;
        }
//        std::cerr<<"After wait on condition : "<<pthread_self()<<std::endl;

        m_waiting_counter --;
        if(m_waiting_counter < 0)
            std::cerr<<"CPP ERROR ! : "<<m_waiting_counter<<std::endl;
        if(pthread_mutex_unlock(&m_mutex) < 0 )
        {
            LOG_INSIDE_ERROR<<"Unlock Mutex error in database manager !"<<LOG_END;
            return NULL;
        }
        
        conn = ConnectionPool_getConnection(m_ppool);
    }

    if(cnt != 0)
        LOG_INSIDE_INFO<<"Get connection..."<<pthread_self()<<LOG_END;

    DBHandle *handle = new DBHandle(this , conn);
    if(NULL == handle)
    {
        LOG_INSIDE_ERROR<<"DBManager::create DBHandle error !"<<LOG_END;
        ConnectionPool_returnConnection(m_ppool , conn);
        return NULL;
    }

    return handle;
}

*/
