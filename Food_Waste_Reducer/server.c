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

/* portul folosit */
#define PORT 2910

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
    int idThread; // id-ul thread-ului tinut in evidenta de acest program
    int cl;       // descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int main()
{
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr; // mesajul primit de trimis la client
    int sd; // descriptorul de socket
    int pid;
    pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
    int i = 0;

    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    /* utilizarea optiunii SO_REUSEADDR -> Network Programming (part II) - slide 22*/
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date -> initializam cu 0/NULL fiecare parametru a structurii*/
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 2) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }

    //=========================================================================================================================================

    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; // parametru functia executata de thread
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n\n", PORT);
        fflush(stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i++;
        td->cl = client;

        pthread_create(&th[i], NULL, &treat, td);
    }
};

static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    fflush(stdout);
    pthread_detach(pthread_self());
    raspunde((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    return (NULL);
};

void raspunde(void *arg)
{
    int nr, i = 0;
    struct thData tdL;
    tdL = *((struct thData *)arg);

    if (read(tdL.cl, &nr, sizeof(int)) <= 0)
    {
        printf("[Thread %d]\n", tdL.idThread);
        perror("Eroare la read() de la client.\n");
    }

    if (nr == 0)
    {
        printf("[Thread %d]Mesajul de la [client_0] a fost receptionat...\n", tdL.idThread);

        /*pregatim mesajul de raspuns */

        char buffer[1024];
        size_t bytesRead;
        FILE *file = fopen("Donations.txt", "r"); // open a file


        bytesRead = fread(buffer, 1, sizeof(buffer), file);
        if (bytesRead <= 0)
        {
            char *msg = "Din pacate lista este goala...";
            strcpy(buffer, msg);
            if (write(tdL.cl, buffer, strlen(msg)) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
                fclose(file);
                return;
            }
        }
        else
        {
            if (write(tdL.cl, buffer, bytesRead) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n\n");
                fclose(file);
                return;
            }
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
            {
                if (write(tdL.cl, buffer, bytesRead) <= 0)
                {
                    perror("[Thread]Eroare la write() catre client.\n\n");
                    fclose(file);
                    return;
                }
            }
        }
        // printf("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);

        // printf("[Thread %d]Trimitem mesajul inapoi...\n\n", tdL.idThread);
    }
    else
    {

        printf("[Thread %d]Mesajul de la [client_1] a fost receptionat...\n", tdL.idThread);
        printf("[Thread %d]Trimitem mesajul inapoi...  TO BE CONTINUED\n\n", tdL.idThread);

        // /*pregatim mesajul de raspuns */
        // nr++;
        // printf("[Thread %d]Trimitem mesajul inapoi...%d\n", tdL.idThread, nr);

        // /* returnam mesajul clientului */
        // if (write(tdL.cl, &nr, sizeof(int)) <= 0)
        // {
        //     printf("[Thread %d] ", tdL.idThread);
        //     perror("[Thread]Eroare la write() catre client.\n");
        // }
        // else
        //     printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);
    }
}