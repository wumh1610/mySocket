#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define     BUF_SIZE    1024
#define     MAXEVENTS   32

static int socket_and_bind(){
    int fd, ret;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.220.135");

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));

    return fd;
}

int main(int argc, char const *argv[])
{
    int sockfd, epfd, cfd, nc, ret, i, clientaddrLen;
    struct epoll_event event, events[MAXEVENTS];
    struct sockaddr clientAddr;
    clientaddrLen = sizeof(struct sockaddr);
    char buf[BUF_SIZE] = {0};

    sockfd = socket_and_bind();

    ret = listen(sockfd, SOMAXCONN);

    epfd = epoll_create(1);

    event.events = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = sockfd;

    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);

    while(1){
        nc = epoll_wait(epfd, events, MAXEVENTS, -1);
        for(i=0; i<nc; i++){

            if((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR) ||
             !(events[i].events & EPOLLIN)){
                 perror("events sockfd error!");
                 close(events[i].data.fd);
                 continue;
             }
             
             if(events[i].data.fd == sockfd){
                 cfd = accept(sockfd, &clientAddr, &clientaddrLen);
                 if(cfd == -1){
                     if(errno == EAGAIN || errno == EWOULDBLOCK){
                         break;
                     }
                     else{
                         perror("accept");
                         continue;
                     }
                 }
                 event.data.fd = cfd;
                 event.events = EPOLLIN | EPOLLET;
                 ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &event);
             }else{
                 cfd = events[i].data.fd;
                 memset(buf, 0, sizeof(buf));
                 ret = read(cfd, buf, sizeof(buf));
                 if(ret < 0){
                     close(cfd);
                 }else if(ret == 0){
                     ret = epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, &event);
                     close(cfd);
                 }else{
                     printf("received from client : %s\n", buf);
                     memset(buf, 0, sizeof(buf));
                     strcpy(buf, "hello client!");
                     ret = write(cfd, buf, sizeof(buf));
                 }
             }
        }
    }
  
    return 0;
}
