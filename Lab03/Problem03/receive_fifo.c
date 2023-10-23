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

    int fd = open("fifo.txt", O_RDONLY);

    char buff[100];
    int len = read(fd, buff, 100);
    buff[len] = '\0';

    write(1, buff, len);

    if (remove("fifo.txt") == 0)
        write(1, "The fifo file has been deleted\n", 30);
    else
        perror("ERR!");

    return 0;
}