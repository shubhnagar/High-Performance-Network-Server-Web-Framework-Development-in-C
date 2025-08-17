#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>

void *worker_thread(void *arg){

    int accept_fd=*(int *)arg;
    free(arg);
    char buffer[1024];
    read(accept_fd,buffer,1024);
    send(accept_fd,"World",5,0);
    close(accept_fd);
    pthread_exit(NULL);
}

int main(int argc,char* argv[]) {
    if(argc!=2){
        printf("Usage <port>\n");
        return -1;
    }
    int port=atoi(argv[1]);
    int opt=1;
    struct sockaddr_in serveradd;
    int socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd<0){
        perror("Socket creation failed!!");
        return -1;
    }

    memset(&serveradd,0,sizeof(struct sockaddr_in));

    serveradd.sin_family=AF_INET;
    serveradd.sin_port=htons(port);
    serveradd.sin_addr.s_addr=INADDR_ANY;
    
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(socket_fd,(struct sockaddr*)&serveradd,sizeof(struct sockaddr))<0) {
        perror("Bind failed!");
        return -1;
    }
    if(listen(socket_fd,1024)<0){
        perror("listen failed!!");
        return -1;
    }
    printf("Server listning on port %d \n",port);
    for(;;){
            pthread_t thread;
            int *accept_fd=malloc(4);
            *accept_fd=accept(socket_fd,NULL,NULL);
            if(pthread_create(&thread,NULL,worker_thread,accept_fd)!=0){
                perror("Thread creation failed!!");
                return -1;
            }
            pthread_detach(thread);
    }
    close(socket_fd);
    return 0;
}