#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
using namespace std;

#define BUF_SIZE 1024

int main(int argc, char const *argv[])
{
    int s[2] = {0};
    int i = 0, ret = 0;
    char buf[BUF_SIZE] = {0};
    char *p = NULL;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) == -1)
    {
        printf("create unnamed socket pair failed:%s\n", strerror(errno));
        return -1;
    }

    pid_t pid[4];

    for (i = 0; i < 4; i++)
    {
        pid[i] = fork();
        switch (pid[i])
        {
        case -1:
            perror("fork()");
            return -1;
        case 0:
            close(s[0]);
            ret = read(s[1], buf, BUF_SIZE);
            cout << buf << " : " << getpid() << ":" << ret << endl;
            return 0;
        default:
        {
            if (i == 3)
            {
                //close(s[1]);
                p = "12345678";
            }
            ret = write(s[0], p, strlen(p));
            cout << "write strlen() = " << strlen(p) << ":" << ret << endl;
        }
        }
    }

    return 0;
}
