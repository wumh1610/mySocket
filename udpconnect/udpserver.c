#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    struct sockaddr_in clientAddr, addr;
    int clientAddrLen, fd, ret;
    char buf[BUF_SIZE];

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.220.135");

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1){
        perror("socket");
        return fd;
    }

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(ret == -1){
        perror("bind");
        return ret;
    }

    while(1){
        memset(buf, 0, BUF_SIZE);
        clientAddrLen = sizeof(struct sockaddr);
        ret = recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr*)&clientAddr, &clientAddrLen);
        printf("receive from client : %s\n", buf);
        memset(buf, 0, BUF_SIZE);
        strcpy(buf, "============123456===========");
        ret = sendto(fd, buf, sizeof(buf), 0, (struct sockaddr*)&clientAddr, clientAddrLen);
    }



    return 0;
}
