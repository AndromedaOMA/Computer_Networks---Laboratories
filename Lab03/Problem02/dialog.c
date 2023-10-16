/*Scrieti un program care citeste in mod continuu input de la utilizator.
  Procesul parinte isi va crea la inceputul executiei un proces copil.
  Input-ul citit de la utilizator il va trimite in mod continuu procesului copil.
  Procesul copil va procesa acel input (alegeti o forma de procesare care sa modifice cumva mesajul) si il va trimite inapoi.
  Procesul parinte va fi responsabil sa afiseze mesajul post-procesare.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int parent_to_child[2];
    int child_to_parent[2];

    pipe(parent_to_child);
    pipe(child_to_parent);

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("ERR!");
        exit(1);
    }
    else if (pid == 0)
    {
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        char buff_from_parent[100];
        int len = read(parent_to_child[0], buff_from_parent, 100);
        buff_from_parent[len] = '\0';

        strcat(buff_from_parent,"The process child readed this!\n");

        write(child_to_parent[1],buff_from_parent,100);

        exit(0);
    }
    else
    {
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        char buff[100];
        read(0, buff, 100);

        write(parent_to_child[1], buff, 100);

        char buff_from_child[100];
        int len = read(child_to_parent[0], buff_from_child, 100);
        buff_from_child[len]='\0';

        write(1,buff_from_child,100);

        wait(NULL);
    }
}