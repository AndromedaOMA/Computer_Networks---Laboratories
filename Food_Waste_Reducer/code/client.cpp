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
    char buf[10];

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
        perror("Error at the socket() function!\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    server.sin_family = AF_INET;                 // familia socket-ului
    server.sin_addr.s_addr = inet_addr(argv[1]); // adresa IP a serverului
    server.sin_port = htons(atoi(argv[2]));      // portul de conectare

    //========================================================================================================================================= Role selection (0/1)

    if (atoi(argv[3]) == 0)
    { //-----------------------------------------------> 0 for charitable organization/people in need [client_0]
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("\n[client_0]Error at the connect() function!\n\n");
            return errno;
        }

        printf("\n[client_0]Hello! You have successfully logged in!\n\n");

        pthread_mutex_lock(&mutex);
        nr = 0;
        if (write(sd, &nr, sizeof(int)) <= 0) // write_1 - role selected
        {
            perror("\n[client_0]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        printf("\n[client_0]Today we have the following list of available donations:\n");
        char buffer[10000];
        int bytesRead;
        while ((bytesRead = read(sd, buffer, sizeof(buffer) - 1)) > 0) // read_1.1 / read_1.2 - list_of_donations
        {
            buffer[bytesRead] = '\0';
            if (strstr(buffer, "end") != NULL)
                break;
            printf("%s", buffer);
        }

        if (strstr(buffer, "Unfortunately") != NULL)
        {
            close(sd);
            return 0;
        }

        //---------test----
        // printf("\n -----TEST: %s\n", buffer);
        //-----------------
        if (bytesRead < 0)
        {
            perror("\n[client_0]Error at the read() function!\n\n");
            return errno;
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
        if (write(sd, &value, sizeof(int)) <= 0) // write_2 - donation_value selected
        {
            perror("\n[client_0]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        printf("\n[client_0]Below you will see the selected donation:\n");
        char buffer_2[10000];
        int bytesRead_2;
        while ((bytesRead_2 = read(sd, buffer_2, sizeof(buffer_2) - 1)) > 0) // read_1.1 / read_1.2 - list_of_donations
        {
            buffer_2[bytesRead_2] = '\0';
            if (strstr(buffer_2, "end") != NULL)
                break;
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

        //......
        printf("\n[client_0]Waiting for the server response and for the accept/refuse of the client_1...\n");
        printf("\nTODO!\n");

        /* inchidem conexiunea, am terminat */
        close(sd);
        return 0;
        //-----------------------------------------------------------------------------------------------------------------------------------------
    }
    else if (atoi(argv[3]) == 1)
    { //-----------------------------------------------> 1 for restaurants/shops [client_1]
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("[client]Error at the connect() function!\n\n");
            return errno;
        }

        printf("[client_1]Hello! You have successfully logged in!\n\n");
        /* citirea mesajului */

        pthread_mutex_lock(&mutex);
        nr = 1;
        if (write(sd, &nr, sizeof(int)) <= 0) // write_1 - role selected
        {
            perror("[client_1]Error at the write() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&mutex);
        printf("\n[client_1]Today we have the following list of requested donations:\n");
        char buffer[10000];
        int bytesRead;
        while ((bytesRead = read(sd, buffer, sizeof(buffer) - 1)) > 0) // read_1.1 / read_1.2 - list_of_requests
        {
            buffer[bytesRead] = '\0';
            if (strstr(buffer, "end") != NULL)
                break;
            printf("%s", buffer);
        }
        //---------test----
        // printf("\n TEST:\n%s\n", buffer);
        //-----------------
        if (bytesRead < 0)
        {
            perror("\n[client_0]Error at the read() function!\n\n");
            return errno;
        }
        pthread_mutex_unlock(&mutex);
        printf("\n");

        /* inchidem conexiunea, am terminat */
        // TODO: INCHIDEM CONNEXIUNEA
        close(sd); //?
    }
    else
        printf("<role> = 0 if you are a charity or a person in need / 1 if you are a restaurant or shop!\n\n");

    close(sd);
    return 0;
}