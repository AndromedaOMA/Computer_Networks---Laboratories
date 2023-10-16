#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    int value = 10;

    printf("Parent process: \n");

    for (int i = 0; i <= 2; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("ERR!");
            exit(1);
        }
        else if (pid == 0)
        {
            printf("Proces copil %d cu PID %d asteapta %d secunde\n", i, getpid(), 2+i);
            sleep(2 + i);
            exit(0);
        }
    }
    for (int i = 0; i <= 2; i++)
    {
        pid_t terminated_pid = waitpid(-1, NULL, 0);
        printf("Waited for the pid %d.\n", terminated_pid);
    }

    return 0;
}