#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <event.h>
#include <event2/util.h>

#define BUFSIZE 1024

int socket_and_bind();
void lis_sfd_cb(int sfd, short events, void *arg);
void client_cb(int cfd, short events, void *arg);

int main(int argc, char const *argv[])
{
    int sfd = socket_and_bind();

    struct event_base *base = event_base_new();
    struct event *ev = event_new(base, sfd, EV_READ | EV_PERSIST, lis_sfd_cb, (void *)base);
    event_add(ev, NULL);

    event_base_dispatch(base);

    event_base_free(base);

    return 0;
}

int socket_and_bind()
{
    int sfd, ret;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        perror("socket");
        exit(sfd);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    // addr.sin_addr.s_addr = inet_addr("192.168.43.178");
    addr.sin_addr.s_addr = 0;

    evutil_make_listen_socket_reuseable(sfd);

    ret = bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        perror("bind");
        //evutil_closesocket(sfd);
        close(sfd);
        exit(ret);
    }

    ret = listen(sfd, SOMAXCONN);
    if (ret < 0)
    {
        perror("listen");
        //evutil_closesocket(sfd);
        close(sfd);
        exit(ret);
    }

    evutil_make_socket_nonblocking(sfd);

    return sfd;
}

void lis_sfd_cb(int sfd, short events, void *arg)
{
    int cfd, clientAddrLen;
    struct sockaddr clientAddr;
    clientAddrLen = sizeof(struct sockaddr);

    cfd = accept(sfd, &clientAddr, &clientAddrLen);
    if (cfd < 0)
    {
        perror("accept");
        return;
    }

    evutil_make_socket_nonblocking(cfd);

    struct event_base *base = (struct event_base *)arg;

    // ev = event_new(base, cfd, EV_READ | EV_PERSIST, client_cb, (void*)ev);

    struct event* ev = (struct event*)malloc(sizeof(struct event));
    event_assign(ev, base, cfd, EV_READ | EV_PERSIST, client_cb, (void *)ev);

    event_add(ev, NULL);
}

void client_cb(int cfd, short events, void *arg)
{
    char buf[BUFSIZE] = {0};
    int ret, len;
    struct event *ev = (struct event *)arg;

    len = read(cfd, buf, sizeof(buf) - 1);
    if (len <= 0)
    {
        perror("read");
        printf("one of clients closed!\n");
        event_free(ev);
        //evutil_closesocket(cfd);
        close(cfd);
        return;
    }

    buf[len] = '\0';
    ret = write(cfd, buf, strlen(buf));
}
