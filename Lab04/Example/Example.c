#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void)
{

    int sockets[2], child;
    char buf[1024];

    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
    child = fork();

    if (child)
    {                      // parent
        close(sockets[0]); // close child's socket

        read(sockets[1], buf, 1024);
        printf("Parent process: %s\n", buf);

        write(sockets[1], "first", 5);

        close(sockets[1]); // close descriptor when no longer needed
    }
    else
    {                      // child
        close(sockets[1]); // close parent's socket


        read(sockets[0], buf, 1024);
        printf("Child process: %s\n", buf);
        
        write(sockets[0], "second", 6);
        
        close(sockets[0]);
    }
    return 0;
}
