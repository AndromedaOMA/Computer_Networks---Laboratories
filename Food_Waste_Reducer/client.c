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
        printf("Sintaxa: %s <adresa_server> <port> <role>\n", argv[0]);
        return -1;
    }

    /* cream socketul */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons(atoi(argv[2]));

    //=========================================================================================================================================

    //-----------------------------------------------> Role selection (0/1)
    if (atoi(argv[3]) == 0)
    { //-----------------------------------------------> 0 for charitable organization/people in need [client_0]
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("[client]Eroare la connect().\n");
            return errno;
        }

        /* citirea mesajului */
        printf("[client_0]Salut! Te-ai conectat cu succes!\n");

        nr = 0;
        if (write(sd, &nr, sizeof(int)) <= 0)
        {
            perror("[client_0]Eroare la write() spre server.\n");
            return errno;
        }

        char fd_context[2048];
        if (read(sd, &fd_context, sizeof(fd_context)) < 0)
        {
            perror("[client_0]Eroare la read() de la server.\n");
            return errno;
        }
        printf("[client_0]Astazi avem urmatoarea lista de donatii disponibile:\n");
        printf("%s\n", fd_context);

        printf("\n");

        //--------------------------------------------------------------------------------------------
        // printf("[client_0]Introduceti un numar: ");
        // fflush(stdout);
        // read(0, buf, sizeof(buf));
        // nr = atoi(buf);
        // // scanf("%d",&nr);

        // printf("[client_0] Am citit %d\n", nr);

        // /* trimiterea mesajului la server */
        // if (write(sd, &nr, sizeof(int)) <= 0)
        // {
        //     perror("[client_0]Eroare la write() spre server.\n");
        //     return errno;
        // }

        // /* citirea raspunsului dat de server
        //    (apel blocant pina cind serverul raspunde) */
        // if (read(sd, &nr, sizeof(int)) < 0)
        // {
        //     perror("[client_0]Eroare la read() de la server.\n");
        //     return errno;
        // }
        // /* afisam mesajul primit */
        // printf("[client_0]Mesajul primit este: %d\n", nr);

        // /* inchidem conexiunea, am terminat */
    }
    else if (atoi(argv[3]) == 1)
    { //-----------------------------------------------> 1 for restaurants/shops [client_1]
        /* ne conectam la server */
        if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("[client]Eroare la connect().\n");
            return errno;
        }

        /* citirea mesajului */
        printf("[client_1]Salut! Te-ai conectat cu succes!\n");

        nr = 1;
        if (write(sd, &nr, sizeof(int)) <= 0)
        {
            perror("[client_0]Eroare la write() spre server.\n");
            return errno;
        }

        printf("\n");

        // printf("[client_1]Introduceti un numar: ");
        // fflush(stdout);
        // read(0, buf, sizeof(buf));
        // nr = atoi(buf);
        // // scanf("%d",&nr);

        // printf("[client_1] Am citit %d\n", nr);

        // /* trimiterea mesajului la server */
        // if (write(sd, &nr, sizeof(int)) <= 0)
        // {
        //     perror("[client_1]Eroare la write() spre server.\n");
        //     return errno;
        // }

        // /* citirea raspunsului dat de server
        //    (apel blocant pina cind serverul raspunde) */
        // if (read(sd, &nr, sizeof(int)) < 0)
        // {
        //     perror("[client_1]Eroare la read() de la server.\n");
        //     return errno;
        // }
        // /* afisam mesajul primit */
        // printf("[client_1]Mesajul primit este: %d\n", nr);

        // /* inchidem conexiunea, am terminat */
    }
    else
        printf("Pentru a selecta un rol, trebuie sa scrieti 0 daca sunteti o organizatie caritabila sau 1 daca sunteti un restaurant\n\n");

    close(sd);
}