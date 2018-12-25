#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int ret, fd;
    char buf[BUF_SIZE] = {0};

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.220.135");

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

    strcpy(buf, "---------hello, I`m client-----------");
    send(fd, buf, strlen(buf), 0);
    memset(buf, 0, sizeof(buf));

    recv(fd, buf, BUF_SIZE, 0);
    printf("receive from server: %s\n", buf);


    return 0;
}
