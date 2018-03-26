#include <iostream>
#include <sys/socket.h> 
#include <sys/epoll.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h>

#define MAXLINE 10 

void setnonblocking(int sock){
   int opts;
   opts = fcntl(sock, F_GETFL);
   if(opts < 0){
      perror("fcntl(sock, GETFL)");
      exit(1);
   }
   opts = opts | O_NONBLOCK;
   if(fcntl(sock, F_SETFL, opts) < 0){
      perror("fcntl(sock,SETFL,opts)");
      exit(1);
   }
}

int main(int argc,char* argv[]){
    if(argc <= 2){
        printf("usage: %s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int listenfd, connfd, sockfd, epfd, nfds; 
    int ret = 0;
    struct sockaddr_in serveraddr;
    bzero(&server_addr,sizeof server_addr);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&server_addr.sin_addr);
    server_addr.sin_port = htons(port);
    listenfd = socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd >= 0);
    ret = bind(listenfd,(struct sockaddr*)&server_addr,sizeof server_addr);
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);
    setnonblocking(listenfd);       //把用于监听的socket设置为非阻塞方式
    //声明epoll_event结构体的变量, ev用于注册事件, events数组用于回传要处理的事件
    struct epoll_event ev,events[20];
    //生成用于处理accept的epoll专用的文件描述符, 指定生成描述符的最大范围为256 
    epfd = epoll_create(256);
    ev.events = EPOLLIN | EPOLLET;  //设置要处理的事件类型
    ev.data.fd = listenfd;          //设置与要处理的事件相关的文件描述符
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);     //注册epoll事件
    struct sockaddr_in clientaddr; 
    socklen_t clilen;
    char line[MAXLINE];
    ssize_t n; 
    for(;;) { 
      nfds = epoll_wait(epfd, events, 20, 500); //等待epoll事件的发生
      for(int i = 0; i < nfds; ++i) {               //处理所发生的所有事件
         if(events[i].data.fd == listenfd){      //监听事件
            connfd = accept(listenfd, (sockaddr*)&clientaddr, &clilen); 
            if(connfd < 0){
               perror("connfd<0");
               exit(1);
            }
            setnonblocking(connfd);           //把客户端的socket设置为非阻塞方式
            char *str = inet_ntoa(clientaddr.sin_addr);
            std::cout << "connect from " << str  <<std::endl;
            ev.data.fd=connfd;                //设置用于读操作的文件描述符
            ev.events=EPOLLIN | EPOLLET;      //设置用于注测的读操作事件
            epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);//注册ev事件
         } else if(events[i].events&EPOLLIN) {     //读事件
            if ((sockfd = events[i].data.fd) < 0) {
               continue;
            }
            if ( (n = read(sockfd, line, MAXLINE)) < 0) {   
               if (errno == ECONNRESET) {
                  close(sockfd);
                  events[i].data.fd = -1; 
               } else {
                  std::cout<<"readline error"<<std::endl;
               }
            } else if (n == 0) {
               close(sockfd); 
               events[i].data.fd = -1; 
            }
            ev.data.fd=sockfd;              //设置用于写操作的文件描述符
            ev.events=EPOLLOUT | EPOLLET;   //设置用于注册的写操作事件 
            epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
         } else if(events[i].events&EPOLLOUT) {　//写事件
            sockfd = events[i].data.fd;
            write(sockfd, line, n);
            ev.data.fd = sockfd;               //设置用于读操作的文件描述符
            ev.events = EPOLLIN | EPOLLET;     //设置用于注册的读操作事件
            epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
         } 
      }
   }
}
