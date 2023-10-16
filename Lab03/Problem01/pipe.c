#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

void process_child(int read_fd, int write_fd)
{
    char buff[10];
    int len_read = read(read_fd, buff, 10);
    buff[len_read] = '\0';

    printf("Read from parent: %s\n", buff);

    write(write_fd, "stuff2", 6);

    exit(0);
}

void process_parent(int read_fd, int write_fd)
{
    write(write_fd, "stuff", 5);

    char buff[10];
    int len_read = read(read_fd, buff, 10);
    buff[len_read] = '\0';

    printf("Read from child: %s\n", buff);

    wait(NULL);
}

int main()
{
    int parent_to_child[2];
    int child_to_parent[2];

    pipe(parent_to_child);
    pipe(child_to_parent);

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("ERR!");
        exit(1);
    }

    if (pid == 0)
    { // child
        close(parent_to_child[WRITE]);
        close(child_to_parent[READ]);

        process_child(parent_to_child[READ], child_to_parent[WRITE]);
    }
    else
    { // parent
        close(parent_to_child[READ]);
        close(child_to_parent[WRITE]);

        process_parent(child_to_parent[READ], parent_to_child[WRITE]);
    }
}