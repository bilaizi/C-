#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

int setnonblocking(int fd){
    int old_option = fcntl(fd,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

void addfd(int epollfd,int fd,bool enable_et){
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(enable_et)
        event.events |= EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);
}

void lt(epoll_event *events, int nfds, int epollfd, int listenfd){
    char buf[BUFFER_SIZE];
    for(int i = 0; i < nfds; i++){
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd){
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_address_length);
            addfd(epollfd,connfd,false);  // 对connfd使用默认的lt模式
        }
        else if(events[i].events & EPOLLIN){ //只要socket读缓存中还有未读的数据，这段代码就会触发
            printf("event trigger once\n");
            memset(buf,'\0',BUFFER_SIZE);
            int ret = recv(sockfd,buf,BUFFER_SIZE-1,0);
            if( ret <= 0 ){
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n",ret,buf);
        }
        else
            printf( "something else happened \n" );
    }
}

void et(epoll_event *events, int nfds, int epollfd, int listenfd){
    char buf[BUFFER_SIZE];
    for(int i = 0; i < nfds; i++){
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd){
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int connfd = accept(listenfd,(struct sockaddr*)&client_address,&client_address_length);
            addfd(epollfd,connfd, true);
        }
        else if (events[i].events & EPOLLIN){
            printf("event trigger once\n");
            while(true){
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
                if(ret < 0){
                    if((errno == EAGAIN )||(errno == EWOULDBLOCK)){
                        printf( "read later\n" );
                        break;
                    }
                    close(sockfd);
                    break;
                }
                else if(ret == 0)
                    close(sockfd);
                else
                    printf("get %d bytes of content: %s\n",ret,buf);
            }
        }
        else
            printf( "something else happened \n" );
    }
}

int main(int argc,char* argv[]){
    if(argc <= 2){
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int ret = 0;
    struct sockaddr_in server_address;
    bzero(&server_address,sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&server_address.sin_addr);
    server_address.sin_port = htons(port);
    int listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd >= 0);
    ret = bind(listenfd, (struct sockaddr*)&server_address, sizeof(server_address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd,listenfd,true);
    while(true){
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0){
            printf("epoll failure\n");
            break;
        }
        lt(events, ret, epollfd, listenfd);
        //et(events,ret,epollfd,listenfd);
    }
    close(listenfd);
    return 0;
}
