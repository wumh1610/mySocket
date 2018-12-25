#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define BUF_SIZE 1024
#define CLIENTS  30

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr;
    fd_set readfds;
    int fd, cfd, maxfd, clientSockets[CLIENTS], addrLen, ret, i;
    char *buf[BUF_SIZE] = {0};

    fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.43.178");

    ret = bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));

    ret = listen(fd, SOMAXCONN);

    memset(clientSockets, 0, sizeof(clientSockets));
    addrLen = sizeof(struct sockaddr);

    while(1){
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        maxfd = fd;

        for(i=0; i<CLIENTS; i++){
            cfd = clientSockets[i];

            if(cfd > 0) FD_SET(cfd, &readfds);

            if(cfd > maxfd) maxfd = cfd;
        }

        ret = select(maxfd+1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(fd, &readfds)){
            cfd = accept(fd, (struct sockaddr*)&addr, &addrLen);
            for(i=0; i<CLIENTS; i++){
                if(clientSockets[i] == 0){
                    clientSockets[i] = cfd;
                    break;
                }
            }
        }

        for(i=0; i<CLIENTS; i++){
            cfd = clientSockets[i];
            if(FD_ISSET(cfd, &readfds)){
                memset(buf, 0, sizeof(buf));
                ret = read(cfd, buf, sizeof(buf));

                if(ret == 0){
                    printf("client closed\n");
                    close(cfd);
                    clientSockets[i] = 0;
                }else{
                    printf("received from client : %s\n", buf);
                    memset(buf, 0, sizeof(buf));
                    strcpy(buf, "hello client, I`m server");
                    write(cfd, buf, sizeof(buf));
                }
                
            }
        }

    }

    return 0;
}
