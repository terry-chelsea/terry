#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/time.h>

#define gettid()  syscall(SYS_gettid)

void *func(void *args)
{
    int para = (int)args;
    printf("get parameter : %d , tid : %d\n" , para , gettid());

    struct timeval tm;
    gettimeofday(&tm , NULL);

    return NULL;
}

int main()
{
    pthread_t tid = 0;
    if(pthread_create(&tid , NULL , func , (void *)16) < 0)
    {
        perror("create thread failed : ");
        return -1;
    }

    struct timeval tm;
    gettimeofday(&tm , NULL);

    pthread_join(tid , NULL);
    printf("work success...\n");
    return 0;
}
