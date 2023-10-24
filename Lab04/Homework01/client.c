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
    //=============================

    char command[1024], response[1024];

    while (1)
    {
        write(1, "+Insert the command: ", 22);
        int len_command = read(0, command, 1024);
        command[len_command] = '\0';
        
        if (strcmp(command, "quit\n") == 0)
        {
            write(1, "End of service!", 16);
            write(fifo_in, command, 1024);
            return 0;
        }
        write(fifo_in, command, 1024);
        int len_response = read(fifo_out, response, 1024);
        response[len_response] = '\0';
        printf("-Server respnose: %s\n", response);
    }

    // if (remove("fifo_out.txt") == 0)
    //     write(1, "The fifo_out file has been deleted\n", 35);
    // else
    //     perror("ERR!");
}
