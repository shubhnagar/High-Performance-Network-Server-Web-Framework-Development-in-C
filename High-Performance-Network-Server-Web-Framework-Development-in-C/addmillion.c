#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <assert.h>

int account_balance = 0;

pthread_mutex_t mutex;

double GetTime() {
    struct timeval t;
    int rc = gettimeofday(&t, NULL);
    assert(rc == 0);
    return (double) t.tv_sec + (double) t.tv_usec/1e6;
}

void *increment(void* nmilions)
{
    int millions=*(int*)nmilions;
    int l_balance=0; 
    while(millions--){
    for (int i = 0; i < 1000000; i++){   
           l_balance++;          
        }   
    } 
    pthread_mutex_lock(&mutex);
    account_balance+=l_balance;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    double start=GetTime();
    int threadNum = atoi(argv[1]);
    pthread_t th[threadNum];
    int div=2048/threadNum;
    pthread_mutex_init(&mutex, NULL);
    int i;
    for (i = 0; i < threadNum; i++)
    {
        if (pthread_create(th + i, NULL, &increment, &div) != 0)
        {
            perror("Failed to create thread");
            return 1;
        }
        printf("Transaction %d has started\n", i);
    }
    for (i = 0; i < threadNum; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            return -1;
        }
        printf("Transaction %d has finished\n", i);
    }
    printf("Account Balance is : %d\n", account_balance);
    pthread_mutex_destroy(&mutex);
    double end=GetTime();
    printf("Time spent: %f s",end-start);
    return 0;
}