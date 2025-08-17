#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int account_balance = 0;

pthread_mutex_t mutex;

void *increment()
{ 
    for (int i = 0; i < 1000000; i++){
        pthread_mutex_lock(&mutex);
        account_balance++;
        pthread_mutex_unlock(&mutex);
    }
    
}

int main(int argc, char *argv[])
{
    int threadNum = 10;
    pthread_t th[threadNum];
    int i;
    pthread_mutex_init(&mutex, NULL);
    for (i = 0; i < threadNum; i++)
    {
        if (pthread_create(th + i, NULL, &increment, NULL) != 0)
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
            return 2;
        }
        printf("Transaction %d has finished\n", i);
    }
    printf("Account Balance is : %d\n", account_balance);
    pthread_mutex_destroy(&mutex);
    return 0;
}