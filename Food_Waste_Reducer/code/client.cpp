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

/* codul de eroare returnat de anumite apeluri */
extern int errno;

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
            perror("[client]Error at the connect() function!\n\n");
            return errno;
        }

        /* citirea mesajului */
        printf("[client_0]Hello! You have successfully logged in!\n\n");

        nr = 0;
        if (write(sd, &nr, sizeof(int)) <= 0)
        {
            perror("[client_0]Error at the write() function!\n\n");
            return errno;
        }

        char fd_context[2048];
        if (read(sd, &fd_context, sizeof(fd_context)) < 0)
        {
            perror("[client_0]Error at the read() function!\n\n");
            return errno;
        }

        printf("[client_0]Today we have the following list of available donations:\n");
        printf("%s\n", fd_context);

        // printf("[client_0]Alegeti un numarul corespunzator donatiei dorite:\n");
        // int value;
        // read(0, value, sizeof(int));

        printf("\n");

        // /* inchidem conexiunea, am terminat */
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

        nr = 1;
        if (write(sd, &nr, sizeof(int)) <= 0)
        {
            perror("[client_1]Error at the write() function!\n\n");
            return errno;
        }

        char fd_context[2048];
        if (read(sd, &fd_context, sizeof(fd_context)) < 0)
        {
            perror("[client_1]Error at the read() function!\n\n");
            return errno;
        }

        printf("[client_1]You have the request to offer the donation:\n");
        printf("%s\n", fd_context);

        printf("\n");

        /* inchidem conexiunea, am terminat */
    }
    else
        printf("<role> = 0 if you are a charity or a person in need / 1 if you are a restaurant or shop!\n\n");

    close(sd);
}