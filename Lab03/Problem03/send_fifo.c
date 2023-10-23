#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{

    if (access("fifo.txt", F_OK) != -1)
        write(1, "The file already exists.\n", 25);
    else
        mknod("fifo.txt", S_IFIFO | 0666, 0);

    int fd = open("fifo.txt", O_WRONLY);
    write(fd, "mesaj", 5);

    return 0;
}