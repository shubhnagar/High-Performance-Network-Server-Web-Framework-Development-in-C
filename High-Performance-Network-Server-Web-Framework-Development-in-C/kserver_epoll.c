#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/epoll.h>


#define maxclients 100000

int main(int argc,char *argv[]){
    if(argc!=2){
        printf("Usage <port>\n");
        return -1;
    }
    int port=atoi(argv[1]);
    int k=0;
    
    struct epoll_event ev, maxevents[maxclients];
    struct sockaddr_in serveradd;
    int socfd=socket(AF_INET,SOCK_STREAM,0);
    if(socfd<0) {
        perror("No socket Created!");
        return 1;
    }
    memset(&serveradd,0,sizeof(struct sockaddr_in));
    serveradd.sin_family=AF_INET;
    serveradd.sin_port=htons(port);
    int opt=1;
    setsockopt(socfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    serveradd.sin_addr.s_addr=INADDR_ANY;
    int b=bind(socfd,(struct sockaddr *)&serveradd,sizeof(struct sockaddr_in));
    if(b<0) {
        perror("Bind error!!");
        return 1;
    }
    if(listen(socfd,5)<0){
        perror("Listen failed!!");
        return 1;
    }
   int epfd=epoll_create(10);
   ev.events=EPOLLIN;
   ev.data.fd=socfd;
   
   epoll_ctl(epfd,EPOLL_CTL_ADD,socfd,&ev);
   for(;;){
      int nfds=epoll_wait(epfd,maxevents,100000,-1);
      if(nfds>0){
        for(int i=0;i<nfds;++i){
            if(maxevents[i].data.fd==socfd && maxevents[i].events==EPOLLIN){
                int connfd=accept(socfd,NULL,NULL);
                ev.events=EPOLLIN;
                ev.data.fd=connfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
                break;
            }
        }
}
for(int i=0;i<nfds;++i){
            if(maxevents[i].data.fd!=socfd && maxevents[i].events==EPOLLIN){
                char buffer[10];
                k++;
            read(maxevents[i].data.fd,buffer,10);
            send(maxevents[i].data.fd,"World",5,0);
            close(maxevents[i].data.fd);
            epoll_ctl(epfd,EPOLL_CTL_DEL,maxevents[i].data.fd,&maxevents[i]);
            printf("%d\n",k);
            fflush(stdout);
          }
    }
    
    }
    return 0;
}