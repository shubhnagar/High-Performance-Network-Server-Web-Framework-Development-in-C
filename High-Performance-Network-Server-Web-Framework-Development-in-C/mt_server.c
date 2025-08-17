#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


#define buffersize 64

struct taskqueue{
    int tasks[buffersize];
    int front;
    int rear;
    int count;
    bool done;
    pthread_mutex_t mutex;
    pthread_cond_t enque;
    pthread_cond_t deque;
};

int enqueue(struct taskqueue *queue,int acceptfd) {
    pthread_mutex_lock(&queue->mutex);
    while(queue->count==buffersize){
        pthread_cond_wait(&queue->enque,&queue->mutex);
    }
    queue->tasks[queue->rear] =acceptfd;
    queue->rear = (queue->rear + 1) %buffersize;
    queue->count++;
    pthread_cond_signal(&queue->deque);                             
    pthread_mutex_unlock(&queue->mutex);
    return 1;
}

int dequeue(struct taskqueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    int acceptfd;
    while (queue->count == 0) {
        if(queue->done){
            pthread_mutex_unlock(&queue->mutex);
            return -1;
        }
        pthread_cond_wait(&queue->deque, &queue->mutex);     
    }
    acceptfd = queue->tasks[queue->front];
    queue->front = (queue->front + 1) %buffersize;
    queue->count--;
    pthread_cond_signal(&queue->enque);
    pthread_mutex_unlock(&queue->mutex);
    return acceptfd;
}

void *worker_thread(void *arg){

   struct taskqueue *queue=(struct taskqueue *)arg; 
    while(1){
        int acceptfd=dequeue(queue);
        if(acceptfd<0) {
            break;
        }
        char buffer[1024];
        read(acceptfd,buffer,1024);
        send(acceptfd,"World",5,0);
        close(acceptfd);
    }
}

int main(int argc,char *argv[]) {

    if(argc!=3){
        printf("Usage <port> <no of threads> ");
        exit(1);
    }

    int port=atoi(argv[1]);
    int opt=1;
    int num_thread=atoi(argv[2]);
    pthread_t thread[num_thread];
    
    struct taskqueue queue;
    struct sockaddr_in serveradd;

    memset(&serveradd,0,sizeof(struct sockaddr_in));
    serveradd.sin_family=AF_INET;
    serveradd.sin_port=htons(port);
    serveradd.sin_addr.s_addr=INADDR_ANY;

    int socket_fd=socket(AF_INET,SOCK_STREAM,0);

    if(socket_fd<0) {
        perror("No socket creted!!");
        return -1;
    }
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(socket_fd,(struct sockaddr*)&serveradd,sizeof(struct sockaddr))<0){
        perror("Bind Error!!");
        return -1;
    }
    if(listen(socket_fd,1024)){
        perror("lisen error!!");
        return -1;
    }

    queue.done=false;
    queue.front=0;
    queue.count=0;
    queue.rear=0;

    pthread_mutex_init(&queue.mutex,NULL);
    pthread_cond_init(&queue.enque,NULL);
    pthread_cond_init(&queue.deque,NULL);


    for(int i=0;i<num_thread;++i){
        if(pthread_create(thread+i,NULL,worker_thread,&queue)!=0){
            perror("Thread creation error!!");
            continue;
        }
    }

    for(;;){
            int acceptfd=accept(socket_fd,NULL,NULL);
            if(acceptfd<0) {
                perror("Accept failed!!");
                continue;
            }
            enqueue(&queue,acceptfd);
    }

    pthread_mutex_lock(&queue.mutex);
    queue.done =true;
    pthread_cond_broadcast(&queue.enque);
    pthread_cond_broadcast(&queue.deque);
    pthread_mutex_unlock(&queue.mutex);
    
    for (int i = 0; i < num_thread; i++) {
        if(pthread_join(thread[i], NULL)!=0) {
            perror("join failed!!");
            return -1;
        }
    }

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.deque);
    pthread_cond_destroy(&queue.enque);

    close(socket_fd);
    return 0;
}