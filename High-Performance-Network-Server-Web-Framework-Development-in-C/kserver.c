#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc,char* argv[])  {
    if(argc!=2){
        printf("Usage <port>\n");
        return -1;
    }
    int port=atoi(argv[1]);
    int c=0;
    struct sockaddr_in serveradd;
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    memset(&serveradd,0,sizeof(struct sockaddr_in));
    serveradd.sin_family=AF_INET;
    int opt=1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    serveradd.sin_port=htons(port);
    serveradd.sin_addr.s_addr=INADDR_ANY;
    if(bind(sfd,(struct sockaddr*)&serveradd,sizeof(struct sockaddr))<0){
        perror("bind error!!");
        return -1;
    }
    if(listen(sfd,5)<0) {
        perror("Listen error!!");
        return -1;
    }
    int i=0;
    for(;;){
            int afd=accept(sfd,0,0);
            if(afd>0) {
                i++;
                printf("client %d\n",i);
                fflush(stdout);
            }
            char buffer[8];
            recv(afd,buffer,8,0);
            send(afd,"D",1,0);
            close(afd);
    }
    close(sfd);
    return 0;
}