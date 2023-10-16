#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void execute_child_process(int passed_variable) {
        printf("Child process.\n");

        printf("Child process passed variable %d\n", passed_variable);

        passed_variable = 15;

        printf("Child process pid %d\n", getpid());
        printf("Child process parent pid %d\n", getppid());

        exit(passed_variable);
}

void execute_parent_process(int passed_variable) {
        printf("Parent process.\n");

        printf("Parent process passed variable %d\n", passed_variable);

        printf("Parent process pid %d\n", getpid());
        printf("Parent process parent pid %d\n", getppid());

        int child_return;

        wait(&child_return);

        printf("Return status from child: %d\n", WEXITSTATUS(child_return));
        printf("Parent process passed variable %d\n", passed_variable);
}

int main(void) {
        printf("Before fork.\n");
        pid_t pid = fork();

        int passed_variable = 10;

        if(pid == -1) {
                perror("Error forking.");
                exit(1);
        }

        if(pid == 0) { // child process
                execute_child_process(passed_variable);
        }

        if(pid > 0) {
                execute_parent_process(passed_variable);
        }
}