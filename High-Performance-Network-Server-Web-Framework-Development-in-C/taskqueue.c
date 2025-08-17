#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>

#define buffersize 64 

// Global variables

long sum = 0;
long odd = 0;
long even = 0;
long min = INT_MAX;
long max = INT_MIN;


pthread_mutex_t data;

struct pair{
    int burst;
    char type;
};
struct taskqueue{
    struct pair tasks[buffersize];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t enque;
    pthread_cond_t deque;
    bool done;
};

void processtask(long number) {
    sleep(number);
    pthread_mutex_lock(&data);
    sum += number;
    if (number % 2 == 1) {
        odd++;
    } else {
        even++;
    }
    if (number < min) {
        min = number;
    }
    if (number > max) {
        max = number;
    }
    pthread_mutex_unlock(&data);
}

int enqueue(struct taskqueue *queue, struct pair task) {
    pthread_mutex_lock(&queue->mutex);
    while(queue->count==buffersize){
        pthread_cond_wait(&queue->enque,&queue->mutex);
    }
    queue->tasks[queue->rear] = task;
    queue->rear = (queue->rear + 1) %buffersize;
    queue->count++;
    pthread_cond_signal(&queue->deque);                             
    pthread_mutex_unlock(&queue->mutex);
    return 1;
}

struct pair dequeue(struct taskqueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    struct pair p;
    while (queue->count == 0) {
        if(queue->done){
            pthread_mutex_unlock(&queue->mutex);
            return p;
        }
        pthread_cond_wait(&queue->deque, &queue->mutex);     
    }
    p = queue->tasks[queue->front];
    queue->front = (queue->front + 1) %buffersize;
    queue->count--;
    pthread_cond_signal(&queue->enque);
    pthread_mutex_unlock(&queue->mutex);
    return p;
}

void *worker_thread(void *arg) {
    struct taskqueue *queue = (struct taskqueue *)arg;
    while (true) {
        struct pair task = dequeue(queue);
        if(task.type=='p'){
            processtask(task.burst);
            printf("Task %d completed\n", task.burst);
        } 
        else if(task.type=='w'){
            printf("Sleeping thread for %d \n",task.burst);
            sleep(task.burst);
        }
        else if (queue->done) {
            break; 
        }
        
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <infile> <num_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char type;
    long num;
    int num_threads = atoi(argv[2]);

    struct taskqueue queue;
    queue.front = 0;
    queue.rear = 0;
    queue.count = 0;

    pthread_mutex_init(&queue.mutex, NULL);
    pthread_cond_init(&queue.enque, NULL);
    pthread_cond_init(&queue.deque,NULL);
    pthread_mutex_init(&data,NULL);

    
    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        if(pthread_create(threads+i, NULL, worker_thread, (void *)&queue)!=0){
            printf("%d thread creation failed!",i);
        }
    }

    FILE *fin = fopen(argv[1], "r");
    long t;
    fscanf(fin, "%ld\n", &t);
    printf("The number of tasks are: %ld\n", t);

    while (fscanf(fin, "%c %ld\n", &type, &num) == 2) {
        struct pair task;
        task.type=type;
        task.burst=num;
        enqueue(&queue,task);
    }
    fclose(fin);

    queue.done =true;
    pthread_cond_broadcast(&queue.enque);
    pthread_cond_broadcast(&queue.deque);
    
    for (int i = 0; i < num_threads; i++) {
        if(pthread_join(threads[i], NULL)!=0){
            perror("Join Error");
            return -1;
        }
    }

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.deque);
    pthread_cond_destroy(&queue.enque);
    pthread_mutex_destroy(&data);

    printf("Final Results: sum = %ld, odd = %ld, even = %ld, min = %ld, max = %ld\n", sum, odd, even, min, max);
    return (EXIT_SUCCESS);
}
