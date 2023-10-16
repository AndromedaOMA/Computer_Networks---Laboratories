#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void)
{
    printf("Before exec.\n");

    printf("%d\n", getpid());

    execlp("sleep", "sleep", "60", NULL);
    printf("After exec.");
}