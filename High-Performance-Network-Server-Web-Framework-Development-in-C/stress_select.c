#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

#define maxclients 1000000

int main(int argc,char* argv[]){
    if(argc!=2){
        printf("Usage <port>\n");
        return -1;
    }
    int port=atoi(argv[1]);
    int maxfd,clientqueue[maxclients];
    fd_set readset;
    struct timeval time;
    struct sockaddr_in serveradd;
    int opt = 1;
    time.tv_sec=10;
    for(int i=0;i<maxclients;++i) {
        clientqueue[i]=0;
    }
    int socfd=socket(AF_INET,SOCK_STREAM,0);
    setsockopt(socfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(socfd<0) {
        perror("No socket Created!");
        return 1;
    }
    memset(&serveradd,0,sizeof(struct sockaddr_in));
    serveradd.sin_family=AF_INET;
    serveradd.sin_port=htons(port);
    serveradd.sin_addr.s_addr=INADDR_ANY;
    
    if(bind(socfd,(struct sockaddr *)&serveradd,sizeof(struct sockaddr_in))<0) {
        perror("Bind error!!");
        return 1;
    }
    if(listen(socfd,10000)<0){
        perror("Listen failed!!");
        return 1;
    }
    maxfd=socfd;
for(;;){
        FD_ZERO(&readset);
        FD_SET(socfd,&readset);
        for(int i=0;i<maxclients;++i){
            if(clientqueue[i]) 
                FD_SET(clientqueue[i],&readset);

            maxfd=maxfd>clientqueue[i] ? maxfd:clientqueue[i];
    }
    int ready=select(maxfd+1,&readset,NULL,NULL,NULL);
        if(ready<0) {
            perror("select error!!");
            continue;
        } 
            if(FD_ISSET(socfd,&readset)){
                int connecsock=accept(socfd,NULL,NULL);
                if(connecsock<0) {
                    for(int i=0;i<maxclients;++i){
                if(clientqueue[i] && FD_ISSET(clientqueue[i],&readset)){
                send(clientqueue[i],"World",5,0);
                close(clientqueue[i]);
                clientqueue[i]=0;
            }
}
                }
               else {
                for(int i=0;i<maxclients;++i){
                    if(clientqueue[i]==0) {
                        clientqueue[i]=connecsock;
                        printf("Client added on socket %d index %d\n", connecsock, i);
                        break;
                    }
                }
        }
        }

for(int i=0;i<maxclients;++i){
            if(clientqueue[i] && FD_ISSET(clientqueue[i],&readset)){
                char buffer[10];
                read(clientqueue[i],buffer,10);
                send(clientqueue[i],"World",5,0);
                close(clientqueue[i]);
                clientqueue[i]=0;
            }
        }

}
    close(socfd);
    return 0; 
}