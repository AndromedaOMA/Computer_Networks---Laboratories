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
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string>

/* portul folosit */
#define PORT 2910

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
    int idThread; // id-ul thread-ului tinut in evidenta de acest program
    int cl;       // descriptorul intors de accept
} thData;

static void *treat_client_0(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii client_0*/
void response_client_0(void *);

static void *treat_client_1(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii client_1*/
void response_client_1(void *);

//========================================================================================================================================= main code

int main()
{
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr;            // mesajul primit de trimis la client
    int sd;            // descriptorul de socket
    pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Error at the socket() function!\n\n");
        return errno;
    }

    /* utilizarea optiunii SO_REUSEADDR -> Network Programming (part II) - slide 22*/
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date -> initializam cu 0/NULL fiecare parametru a structurii*/
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    server.sin_family = AF_INET;                // stabilirea familiei de socket-uri
    server.sin_addr.s_addr = htonl(INADDR_ANY); // acceptam orice adresa
    server.sin_port = htons(PORT);              // utilizam un port utilizator

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Error at the bind() function!\n\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 2) == -1)
    {
        perror("[server]Error at the listen() function!\n\n");
        return errno;
    }

    //========================================================================================================================================= second main code

    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; // parametru functia executata de thread
        unsigned int length = sizeof(from);

        printf("[server]I'm waiting at the port %d...\n\n", PORT);
        fflush(stdout);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Error at the accept() function!\n\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */
        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;

        struct thData tdL;
        tdL = *((struct thData *)td);
        int nr, i = 0;

        if (read(tdL.cl, &nr, sizeof(int)) <= 0)
        {
            printf("[Thread %d]\n", tdL.idThread);
            perror("Error at the read() function!\n\n");
        }
        if (nr == 0)
            pthread_create(&th[i], NULL, &treat_client_0, td);
        else
            pthread_create(&th[i], NULL, &treat_client_1, td);
    }
};

//========================================================================================================================================= client_0 code

static void *treat_client_0(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    fflush(stdout);
    pthread_detach(pthread_self());
    response_client_0((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    return (NULL);
};

void response_client_0(void *arg)
{
    int nr, i = 0;
    struct thData tdL;
    tdL = *((struct thData *)arg);

    printf("[Thread %d]The message from [client_0] has been received...\n\n", tdL.idThread);

    /*pregatim mesajul de raspuns */

    char buffer[1024];
    size_t bytesRead;
    FILE *file = fopen("Donations.txt", "r"); // open a file


    bytesRead = fread(buffer, 1, sizeof(buffer), file);
    if (bytesRead <= 0)
    {
        std::string msg = "Unfortunately, the list of donations is empty...";
        buffer==msg;
        if (write(tdL.cl, buffer, msg.length()) <= 0)
        {
            perror("[Thread]Error at the write() function!\n\n");
            fclose(file);
            return;
        }
    }
    else
    {
        if (write(tdL.cl, buffer, bytesRead) <= 0)
        {
            perror("[Thread]Error at the write() function!\n\n");
            fclose(file);
            return;
        }
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
        {
            if (write(tdL.cl, buffer, bytesRead) <= 0)
            {
                perror("[Thread]Error at the write() function!\n\n");
                fclose(file);
                return;
            }
        }
    }
}

//========================================================================================================================================= client_1 code

static void *treat_client_1(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    fflush(stdout);
    pthread_detach(pthread_self());
    response_client_1((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    return (NULL);
};

void response_client_1(void *arg)
{
    // int nr, i = 0;
    struct thData tdL;
    tdL = *((struct thData *)arg);

    printf("[Thread %d]The message from [client_1] has been received...\n\n", tdL.idThread);
    // printf("[thread]- %d - I'm waiting for the message...\n", tdL.idThread);

    /*pregatim mesajul de raspuns */
    char buffer[1024];
    FILE *file = fopen("Donations.txt", "r"); // open a file

    int lineNumberToRead = 3;
    char line[1000];
    for (int currentLine = 1; currentLine <= lineNumberToRead; ++currentLine)
        if (fgets(line, sizeof(line), file) == NULL)
        {
            printf("[Thread %d]The line %d does not exist in the file.\n\n", tdL.idThread, lineNumberToRead);
            fclose(file);
            return;
        }

    // Afiseaza linia citita
    if (write(tdL.cl, line, sizeof(line)) <= 0)
    {
        perror("[Thread]Error at the write() function!\n\n");
        fclose(file);
        return;
    }

    // printf("[Thread %d]Trimitem mesajul inapoi...\n\n", tdL.idThread);
}
//=========================================================================================================================================