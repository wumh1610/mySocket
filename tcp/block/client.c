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
    int fd, ret;
    char* buf[BUF_SIZE] = {0};
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.43.178");

    ret = connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if(ret < 0) exit(-1);

    //while(1){
        memset(buf, 0, BUF_SIZE);
        strcpy(buf, "hello server, I`m client!");
        write(fd, buf, sizeof(buf));
        memset(buf, 0, BUF_SIZE);
        read(fd, buf, sizeof(buf));
        printf("received from server : %s\n", buf);
    //}
    close(fd);
    return 0;
}