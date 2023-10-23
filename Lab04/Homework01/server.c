#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>

#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void)
{
    //----------------------------
    if (access("fifo_in.txt", F_OK) != -1)
        write(1, "The fifo_in.txt file already exists.\n", 38);
    else
        mknod("fifo_in.txt", S_IFIFO | 0666, 0);

    int fifo_in = open("fifo_in.txt", O_RDONLY);
    //----------------------------
    if (access("fifo_out.txt", F_OK) != -1)
        write(1, "The fifo_out.txt file already exists.\n", 39);
    else
        mknod("fifo_out.txt", S_IFIFO | 0666, 0);

    int fifo_out = open("fifo_out.txt", O_WRONLY);
    //-----------------------------

    int ok = 1;
    char command[1024], response[1024];
    int sockets[2], pid;
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
    pid = fork();

    if (pid)
    {                      // parent
        close(sockets[0]); // close child's socket

        read(fifo_in, command, 1024);
        write(sockets[1], command, 1024);
        read(sockets[1], response, 1024);
        write(fifo_out, response, 1024);

        close(sockets[1]); // close descriptor when no longer needed
    }
    else
    {                      // child
        close(sockets[1]); // close parent's socket

        int len = read(sockets[0], command, 1024);
        command[len] = '\0';
        int dim = strlen(command) - 1;
        strncpy(response, command, dim);
        strcat(response, "-verified");
        write(sockets[0], response, 1024);

        close(sockets[0]);
        exit(1);
    }

    // if (remove("fifo_in.txt") == 0)
    //     write(1, "The fifo_in file has been deleted\n", 35);
    // else
    //     perror("ERR!");
    return 0;
}
