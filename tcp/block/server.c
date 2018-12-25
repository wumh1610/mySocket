#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<netinet/in.h>
#include <arpa/inet.h>


#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int fd, cfd, ret;
    char* buf[BUF_SIZE] = {0};
    struct sockaddr_in addr;
    struct sockaddr clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        exit(-1);
    }
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.43.178");

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if(ret < 0) exit(-1);

    ret = listen(fd, SOMAXCONN);
    if(ret < 0) exit(-1);

    while(1){
        cfd = accept(fd, &clientAddr, &clientAddrLen);
        memset(buf, 0, BUF_SIZE);
        read(cfd, buf, sizeof(buf));
        printf("received from client : %s\n", buf);
        memset(buf, 0, BUF_SIZE);
        strcpy(buf, "hello client, I got your message!");
        write(cfd, buf, sizeof(buf));
    }
    
    close(fd);
    return 0;
}
