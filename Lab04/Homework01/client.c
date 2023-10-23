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

    int fifo_in = open("fifo_in.txt", O_WRONLY);
    //----------------------------
    if (access("fifo_out.txt", F_OK) != -1)
        write(1, "The fifo_out.txt file already exists.\n", 39);
    else
        mknod("fifo_out.txt", S_IFIFO | 0666, 0);

    int fifo_out = open("fifo_out.txt", O_RDONLY);
    //-----------------------------

    char command[1024], response[1024];
    int ok = 1;

    while (ok)
    {
        write(1, "Insert the command: ", 21);
        read(0, command, 1024);
        write(fifo_in, command, 1024);
        int len = read(fifo_out, response, 1024);
        response[len] = '\0';
        printf("Server respnose: %s\n", response);

        if (strcmp(response, "quit-verified\0") == 0)
            ok = 0;
    }

    // if (remove("fifo_out.txt") == 0)
    //     write(1, "The fifo_out file has been deleted\n", 35);
    // else
    //     perror("ERR!");
    return 0;
}
