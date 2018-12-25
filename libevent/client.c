#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include<event.h>
#include<event2/util.h>

#define BUFSIZE 1024

int socket_and_connect();
void cfd_read_cb(int fd, short events, void* arg);
void std_write_cb(int fd, short events, void* arg);

int main(int argc, char const *argv[])
{
    int cfd;
    cfd = socket_and_connect();
    if(cfd < 0){
        perror("socket_and_connect");
        return cfd;
    }
    printf("connect server successful!\n");

    struct event_base *base = event_base_new();
    struct event *ev = event_new(base, cfd, EV_READ | EV_PERSIST, cfd_read_cb, NULL);
    event_add(ev, NULL);

    struct event* std_ev = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST,std_write_cb, (void*)&cfd);
    event_add(std_ev, NULL);

    event_base_dispatch(base);

    return 0;
}

int socket_and_connect()
{
    int cfd, ret;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(cfd < 0){
        perror("socket");
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.43.178");

    ret = connect(cfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(ret < 0){
        perror("connect");
        //evutil_closesocket(cfd);
        close(cfd);
        exit(-1);
    }

    evutil_make_socket_nonblocking(cfd);

    return cfd;
}

void std_write_cb(int fd, short events, void* arg)
{
    char buf[BUFSIZE] = {0};
    int ret;
    int cfd = *((int*)arg);
    ret = read(fd, buf, sizeof(buf));
    if(ret < 0){
        perror("read");
        return;
    }

    write(cfd, buf, ret);
}

void cfd_read_cb(int fd, short events, void* arg)
{
    char buf[BUFSIZE] = {0};
    int ret = read(fd, buf, sizeof(buf)-1);
    if(ret <= 0){
        perror("client read!");
        exit(-1);
    }
    buf[ret] = '\0';
    printf("receive from server : %s\n", buf);
}
