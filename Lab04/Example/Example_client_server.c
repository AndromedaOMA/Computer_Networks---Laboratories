#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void)
{

    int sockets[2], server;
    char buf[1024];

    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
    server = fork();

    if (server)
    {                      // server
        close(sockets[1]); // close client's socket

        read(sockets[0], buf, 1024);
        printf("Server process: %s\n", buf);
        write(sockets[0], "first", 5);

        close(sockets[0]); // close descriptor when no longer needed
    }
    else
    {                      // client
        close(sockets[0]); // close server's socket

        write(sockets[1], "second", 6);
        read(sockets[1], buf, 1024);
        printf("Client process: %s\n", buf);

        close(sockets[1]);
    }
    return 0;
}
