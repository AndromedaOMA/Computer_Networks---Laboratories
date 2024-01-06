/*
Convention:
    1. client_0 = charitable organization/people in need
    2. client_1 = restaurants/shops
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <string>

// Declare mutex globally
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* codul de eroare returnat de anumite apeluri */
// extern int errno;

int main(int argc, char *argv[])
{
    int sd;                    // descriptorul de socket
    struct sockaddr_in server; // structura folosita pentru conectare
                               // mesajul trimis
    int nr = 0;
    char username[20];

    /* exista toate argumentele in linia de comanda? */
    if (argc != 4)
    {
        printf("Sintax: %s <server_adress> <port> <role>\n", argv[0]);
        printf("Example: %s 127.0.0.1 2222 0\n", argv[0]);
        return -1;
    }

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\nError at the socket() function!\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    server.sin_family = AF_INET;                 // familia socket-ului
    server.sin_addr.s_addr = inet_addr(argv[1]); // adresa IP a serverului
    server.sin_port = htons(atoi(argv[2]));      // portul de conectare

    if (atoi(argv[3]) == 0) //============================================> 0 for charitable organization/people in need [client_0]
    {
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("\n[client_0]Error at the connect() function!\n");
            return errno;
        }

        pthread_mutex_lock(&mutex);
        nr = 0;
        if (write(sd, &nr, sizeof(int)) <= 0) // 0.write_1 - role selected
        {
            perror("\n[client_0]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        //----------------------------------------------------------------------------------------------------------------------------------------- Donation selection
        pthread_mutex_lock(&mutex);
        char names_buffer[10000];
        int names_bytesRead;
        printf("\n[client_0]Here is the list of usernames that are already registered to us:\n");
        while ((names_bytesRead = read(sd, names_buffer, sizeof(names_buffer) - 1)) > 0) // 0.read_1 - list_of_usernames
            if (strstr(names_buffer, "end") != NULL)
            {
                names_buffer[names_bytesRead - 3] = '\0';
                write(1, names_buffer, strlen(names_buffer));
                break;
            }
            else
            {
                names_buffer[names_bytesRead] = '\0';
                write(1, names_buffer, strlen(names_buffer));
            }
        pthread_mutex_unlock(&mutex);

        //======TEST=====
        // write(0, "\n TEST---: ", 12);
        // write(0, names_buffer, strlen(names_buffer));
        // write(0, "\n", 1);
        //===============

        pthread_mutex_lock(&mutex);
        printf("\n[client_0]Please choose your valid username from the above given list!\n");
        char current_name[20];
        if (scanf("%s", current_name) != 1)
        {
            perror("\n\n[client_0]Error at the scanf() function (reading from terminal)!\n\n");
            return errno;
        }
        write(sd, current_name, sizeof(current_name)); // 0.write_2 - send_username

        char hello_msg[100];
        read(sd, hello_msg, sizeof(hello_msg)); // 0.read_2 - hello_msg
        if (strstr(hello_msg, "not") != NULL)
        {
            printf("\n[client_0]You are not registered in our database!\n");
            return errno;
        }
        else
        {
            printf("\n[client_0]You are registered in our database!\n");
            write(0, hello_msg, strlen(hello_msg));
        }
        pthread_mutex_unlock(&mutex);
        //----verify-------
        // close(sd);
        //-----------------

        //----------------------------------------------------------------------------------------------------------------------------------------- Donation selection

        pthread_mutex_lock(&mutex);
        char buffer[1000];
        int bytesRead;
        printf("\n[client_0]Today we have the following list of available donations:\n");
        while ((bytesRead = read(sd, buffer, sizeof(buffer) - 1)) > 0) // 0.read_3.1 / read_3.2 - list_of_donations
            if (strstr(buffer, "end") != NULL)
            {
                buffer[bytesRead - 3] = '\0';
                write(1, buffer, strlen(buffer));
                break;
            }
            else
            {
                buffer[bytesRead] = '\0';
                write(1, buffer, strlen(buffer));
            }
        //---------test----
        // printf("\n -----TEST: %s\n", buffer);
        //-----------------

        if (strstr(buffer, "Unfortunately") != NULL)
        {
            close(sd);
            return 0;
        }
        pthread_mutex_unlock(&mutex);

        //----------------------------------------------------------------------------------------------------------------------------------------- Donation selection

        printf("\n[client_0]Choose a number corresponding to the desired donation (ID_Donation): ");
        int value;

        if (scanf("%d", &value) != 1)
        {
            perror("\n\n[client_0]Error at the scanf() function (reading from terminal)!\n\n");
            return errno;
        }

        //----test-----------
        // printf("\n TEST\n");
        printf("\n\n[client_0]You have selected the donation with the number: %d\n", value);
        //--------------------

        pthread_mutex_lock(&mutex);
        if (write(sd, &value, sizeof(int)) <= 0) // 0.write_2 - donation_value selected
        {
            perror("\n[client_0]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        printf("\n[client_0]Below you will see the selected donation:\n");
        char buffer_2[10000];
        int bytesRead_2;
        while ((bytesRead_2 = read(sd, buffer_2, sizeof(buffer_2) - 1)) > 0) // 0.read_1.1 / read_1.2 - list_of_donations
            if (strstr(buffer_2, "end") != NULL)
            {
                buffer_2[bytesRead_2 - 3] = '\0';
                printf("%s", buffer_2);
                break;
            }
            else
            {
                buffer_2[bytesRead_2] = '\0';
                printf("%s", buffer_2);
            }
        //---------test----
        // printf("\n TEST:\n%s\n", buffer_2);
        //-----------------
        if (bytesRead_2 < 0)
        {
            perror("\n[client_0]Error at the read() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        // //......
        // printf("\n[client_0]Waiting for the server response and for the accept/refuse of the client_1...\n");
        // printf("\nTODO!\n");
        // sleep(2);

        // /* inchidem conexiunea, am terminat */
        // close(sd);
        // return 0;
        //-----------------------------------------------------------------------------------------------------------------------------------------
    }
    else if (atoi(argv[3]) == 1) //============================================> 1 for restaurants/shops [client_1]
    {
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("\n[client_1]Error at the connect() function!\n\n");
            return errno;
        }

        /* citirea mesajului */
        pthread_mutex_lock(&mutex);
        nr = 1;
        if (write(sd, &nr, sizeof(int)) <= 0) // 1.write_1 - role selected
        {
            perror("\n[client_1]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        //----------------------------------------------------------------------------------------------------------------------------------------- Donation selection

        pthread_mutex_lock(&mutex);
        char names_buffer[10000];
        int names_bytesRead;
        printf("\n[client_1]Here is the list of usernames that are already registered to us:\n");
        while ((names_bytesRead = read(sd, names_buffer, sizeof(names_buffer) - 1)) > 0) // 0.read_1 - list_of_usernames
            if (strstr(names_buffer, "end") != NULL)
            {
                names_buffer[names_bytesRead - 3] = '\0';
                write(1, names_buffer, strlen(names_buffer));
                break;
            }
            else
            {
                names_buffer[names_bytesRead] = '\0';
                write(1, names_buffer, strlen(names_buffer));
            }
        pthread_mutex_unlock(&mutex);

        //======TEST=====
        // write(0, "\n TEST---: ", 12);
        // write(0, names_buffer, strlen(names_buffer));
        // write(0, "\n", 1);
        //===============

        pthread_mutex_lock(&mutex);
        printf("\n[client_1]Please choose your valid username from the above given list!\n");
        char current_name[20];
        if (scanf("%s", current_name) != 1)
        {
            perror("\n\n[client]Error at the scanf() function (reading from terminal)!\n\n");
            return errno;
        }
        write(sd, current_name, sizeof(current_name)); // 0.write_2 - send_username

        char hello_msg[100];
        read(sd, hello_msg, sizeof(hello_msg)); // 0.read_2 - hello_msg
        if (strstr(hello_msg, "not") != NULL)
        {
            printf("\n[client_0]You are not registered in our database!\n");
            return errno;
        }
        else
        {
            printf("\n[client_0]You are registered in our database!\n");
            write(0, hello_msg, strlen(hello_msg));
        }
        pthread_mutex_unlock(&mutex);

        //----------------------------------------------------------------------------------------------------------------------------------------- Donation selection

        pthread_mutex_lock(&mutex);
        printf("\n[client_1]Today we have the following list of requested donations:\n");
        char buffer[10000];
        int bytesRead;
        while ((bytesRead = read(sd, buffer, sizeof(buffer) - 1)) > 0) // 1.read_1.1 / read_1.2 - list_of_requests
            if (strstr(buffer, "end") != NULL)
            {
                buffer[bytesRead - 3] = '\0';
                write(1, buffer, strlen(buffer));
                break;
            }
            else
            {
                buffer[bytesRead] = '\0';
                write(1, buffer, strlen(buffer));
            }
        //---------test----
        // printf("\n TEST:\n%s\n", buffer);
        //-----------------
        if (bytesRead < 0)
        {
            perror("\n[client_1]Error at the read() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        //-----------------------------------------------------------------------------------------------------------------------------------------

        pthread_mutex_lock(&mutex);
        printf("Do you accept the donation requests? (yes/no): ");
        char answer[10];
        if (scanf("%s", answer) != 1)
        {
            perror("\n\n[client_1]Error at the scanf() function (reading from terminal)!\n\n");
            return errno;
        }
        if (write(sd, &answer, sizeof(answer)) <= 0) // 1.write_4
        {
            perror("\n[client_1]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        char final_answer[1000];
        read(sd, &final_answer, sizeof(final_answer)); // 1.read_4
        printf("\n[client_1]The answer from the server is: %s\n", final_answer);

        /* inchidem conexiunea, am terminat */
        close(sd);
    }
    else
        printf("\nThere is a problem in your ./client call!\n<role> = 0 if you are a charity or a person in need / 1 if you are a restaurant or shop!\n\n");

    close(sd);
    return 0;
}