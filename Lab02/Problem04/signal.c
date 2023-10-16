#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include <fcntl.h>

#define MAX_MESSAGE_SIZE 100

void signal_changer1()
{
    printf("U don't know me SON!\n");
}
void signal_changer2()
{
    printf("U can't stop me SON!\n");
}

int main()
{
    signal(SIGUSR1, signal_changer1);
    signal(SIGUSR2, signal_changer2);

    for (int i = 0; i < 2; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("ERR!");
            exit(1);
        }
        else if (pid == 0 && i == 0)
        {
            int descriptor1 = open("text1.txt", O_WRONLY);
            for (int j = 0; j < 2; j++)
            {
                char *message = "Hello, \n";
                write(descriptor1, message, strlen(message));
                sleep(3);
            }
            exit(0);
        }
        else if (pid == 0 && i == 1)
        {
            int descriptor2 = open("text2.txt", O_WRONLY);
            for (int j = 0; j < 2; j++)
            {
                char *message = "World! \n";
                write(descriptor2, message, strlen(message));
                sleep(5);
            }
            exit(0);
        }
    }

    int terminated_pid1 = waitpid(-1, NULL, 0);
    printf("Waited for the pid %d.\n", terminated_pid1);
    kill(getpid(), SIGUSR1);

    int terminated_pid2 = waitpid(-1, NULL, 0);
    printf("Waited for the pid %d.\n", terminated_pid2);
    kill(getpid(), SIGUSR2);

    char *message1 = malloc(MAX_MESSAGE_SIZE);
    int descriptor1 = open("text1.txt", O_RDONLY);
    read(descriptor1, message1, 100);
    write(1, message1, strlen(message1));

    char *message2 = malloc(MAX_MESSAGE_SIZE);
    int descriptor2 = open("text2.txt", O_RDONLY);
    read(descriptor2, message2, 100);
    write(1, message2, strlen(message2));
}