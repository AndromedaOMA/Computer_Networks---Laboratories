#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <utmp.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

bool user_existence_check(char username[], const char fd_name[])
{
    username[strlen(username) - 1] = '\0';
    char list[1024];
    int fd_usernames = open(fd_name, O_RDONLY);
    int len = read(fd_usernames, list, 1024);
    list[len] = '\0';

    //------test--------
    // write(1, "Test!\n", 6);
    // write(1, list, len);
    // write(1, " ", 1);
    // write(1, username, strlen(username));
    // write(1, " ", 1);
    //-------------------

    char *token = strtok(list, " ");
    while (token)
    {
        // write(1, token, strlen(token));
        // write(1, " ", 1);

        // char *aux;
        // strcpy(aux, token);
        // strcat(aux, "\n");
        if (strcmp(token, username) == 0)
            return 1;
        token = strtok(NULL, " ");
    }
    return 0;
}

bool length_validation(char response[]) // good
{
    char first_char = response[0];
    int len = atoi(&first_char);
    char *username = &response[2];
    //---------test-------------
    // printf("    test2 %d %ld ", len, strlen(username));
    //-----------------
    if (len == strlen(username))
        return 1;
    return 0;
}

void get_logged_users(char *buff)
{
    // struct utmp *data;

    // setutent();

    // while (data = getutent())
    // {
    //     char *message1;
    //     strcpy(message1, "Usr: ");
    //     strcat(message1, data->ut_user);
    //     char *message2;
    //     strcpy(message2, ", Hnm: ");
    //     strcat(message2, data->ut_host);
    //     char *message3;
    //     strcpy(message3, ", Time: ");
    //     strcat(message3, data->ut_tv.tv_sec);
    //     strcat(buff, message1);
    //     strcat(buff, message2);
    //     strcat(buff, message3);
    //     strcat(buff, " | ");
    // }

    // endutent();
}

int main(void)
{
    //----------------------------fifo_in
    if (access("fifo_in.txt", F_OK) != -1)
        write(1, "The fifo_in.txt file already exists.\n", 38);
    else
        mknod("fifo_in.txt", S_IFIFO | 0666, 0);

    int fifo_in = open("fifo_in.txt", O_RDONLY);
    //----------------------------fifo_out
    if (access("fifo_out.txt", F_OK) != -1)
        write(1, "The fifo_out.txt file already exists.\n", 39);
    else
        mknod("fifo_out.txt", S_IFIFO | 0666, 0);

    int fifo_out = open("fifo_out.txt", O_WRONLY);
    //----------------------------usernames
    if (access("usernames.txt", F_OK) != -1)
        write(1, "The usernames.txt file already exists.\n", 40);
    else
        mknod("usernames.txt", S_IFREG | 0666, 0);

    int fd_usernames = open("usernames.txt", O_RDONLY | O_WRONLY);
    //==========================================

    int ok = 1, logged = 0;
    char command[1024], response[1024];

    while (ok)
    {
        // strcpy(command, "");
        // strcpy(response, "");

        read(fifo_in, command, 1024);

        if (strcmp(command, "quit\n") == 0)
        {
            write(1, "End of service!", 16);
            return 0;
        }
        else //------------------------------------------------------"login : username" --- DONE!
            if (strncmp(command, "login :", 7) == 0)
            {
                int sockets[2], pid;
                socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
                pid = fork();

                if (pid)
                {                      // parent
                    close(sockets[0]); // close child's socket

                    write(sockets[1], command, strlen(command));
                    int len = read(sockets[1], response, 1024);
                    response[len] = '\0';

                    //--------test--------------
                    // printf(" test3   %d  %s", len, response);
                    //---------------------------

                    if (length_validation(response))
                    {
                        char *message = &response[2];
                        if (strcmp(message, "Succes") == 0)
                            logged = 1;
                        write(fifo_out, message, strlen(message));
                        //--------test--------------
                        // printf(" test4   %d", logged);
                        //---------------------------
                    }
                    else
                        write(fifo_out, "ERR at login!", 13);

                    close(sockets[1]); // close descriptor when no longer needed
                }
                else
                {                      // child
                    close(sockets[1]); // close parent's socket
                    char command_from_parent[1024];

                    int len = read(sockets[0], command_from_parent, 1024);
                    command_from_parent[len] = '\0';

                    char *username = &command_from_parent[8];
                    // ---population---
                    // write(fd_usernames, username, strlen(username));
                    // write(fd_usernames, " ", 1);

                    if (user_existence_check(username, "usernames.txt")) // TO_DO: verif functia! -- DONE
                    {
                        // char *len = strlen("succes") + '0';
                        strcpy(response, "6");
                        strcat(response, ":");
                        strcat(response, "Succes");
                    }
                    else
                    {
                        // char *len = strlen("failed") + '0';
                        strcpy(response, "6");
                        strcat(response, ":");
                        strcat(response, "Failed");
                        // strcpy(response, "16"); // nu stiu de ce nu merge...
                        // strcat(response, ":");
                        // strcat(response, "Failed to login!");
                    }

                    write(sockets[0], response, strlen(response));

                    close(sockets[0]);
                    exit(1);
                }
            }
            else //------------------------------------------------------"get-logged-users"
                if (strncmp(command, "get-logged", 10) == 0)
                {
                    if (logged)
                    {
                        int parent_to_child[2];
                        int child_to_parent[2];

                        pipe(parent_to_child);
                        pipe(child_to_parent);

                        pid_t pid = fork();

                        if (pid)
                        { // parent
                            close(parent_to_child[0]);
                            close(child_to_parent[1]);

                            char buff_from_child[1024];
                            int len = read(child_to_parent[0], buff_from_child, 1024);
                            buff_from_child[len] = '\0';

                            if (length_validation(buff_from_child))
                            {
                                char *message = &buff_from_child[2];
                                write(fifo_out, message, 1022);
                            }
                            else
                                write(fifo_out, "ERR at get-logged-users!", 25);
                        }
                        else
                        { // child
                            close(parent_to_child[1]);
                            close(child_to_parent[0]);

                            char buff_for_parent[1024] = "";
                            get_logged_users(buff_for_parent); // TO_DO: verif functia!

                            write(child_to_parent[1], buff_for_parent, 1024);

                            exit(0);
                        }
                    }
                    else
                    {
                        char *message = "ERR! You're not logged in!";
                        write(fifo_out, message, strlen(message));
                    }
                }
                else //------------------------------------------------------"get-proc-info : pid"
                    if (strncmp(command, "get-proc", 8) == 0)
                    {
                        if (logged)
                        {
                        }
                        else
                        {
                            char *message = "ERR! You're not logged in!";
                            write(fifo_out, message, strlen(message));
                        }
                    }
                    else //------------------------------------------------------"logout" --- DONE!
                        if (strncmp(command, "logout", 6) == 0)
                        {
                            if (logged)
                            {
                                logged = 0;
                                char *message = "Logged out!";
                                write(fifo_out, message, strlen(message));
                                //--------test--------------
                                // printf(" test4   %d", logged);
                                //---------------------------
                            }
                            else
                            {
                                char *message = "ERR! You're not logged in!";
                                write(fifo_out, message, strlen(message));
                            }
                        }
    }

    // if (remove("fifo_in.txt") == 0)
    //     write(1, "The fifo_in file has been deleted\n", 35);
    // else
    //     perror("ERR!");
}
