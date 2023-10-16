#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    pid_t pid = fork();

    if (access("fifo.txt", F_OK) != -1)
        write(1, "The file already exists.\n", 25);
    else
        open("fifo.txt", O_CREAT | O_WRONLY);

    mknod("fifo.txt", S_IFIFO | 0666, 0);

    if (pid == -1)
    {
        perror("ERR!");
        exit(1);
    }
    else if (pid == 0)
    {
        execlp("ls", "ls", NULL);
        exit(0);
    }
    else
    {
    }
}