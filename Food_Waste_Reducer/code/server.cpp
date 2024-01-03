/*
Convention:
    1. client_0 = charitable organization/people in need
    2. client_1 = restaurants/shops
*/
#include <iostream>
#include <vector>
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
#include <sstream>

#include <tuple>
// #include <mariadb/conncpp.hpp>
#include <mariadb/mysql.h>

/* portul folosit */
#define PORT 3336
int OK;

/* codul de eroare returnat de anumite apeluri */
// extern int errno;

typedef struct thData
{
    int idThread; // id-ul thread-ului tinut in evidenta de acest program
    int cl;       // descriptorul intors de accept
} thData;

static void *treat_client_0(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii client_0*/
void request_sent(void *);

static void *treat_client_1(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii client_1*/
void request_list_viewer(void *);

void final_message(void *);

// Declare mutex globally
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*clasa/structura conectiunii C++-MariaDB*/
class SQLConnection
{
private:
    MYSQL *connection;
    std::string server, user, password, database;

public:
    SQLConnection(std::string server, std::string user, std::string password, std::string database)
    {
        this->server = server;
        this->user = user;
        this->password = password;
        this->database = database;

        connection = mysql_init(NULL);

        if (!connection)
        {
            fprintf(stderr, "Connection initialization failed!\n");
            exit(1);
        }

        if (!mysql_real_connect(connection, server.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0))
        {
            fprintf(stderr, "Connection failed!\n");
            exit(1);
        }
    }

    ~SQLConnection()
    {
        if (connection)
            mysql_close(connection);
    }

    MYSQL *getConnection()
    {
        return connection;
    }
};

//=========================================================================================================================================

void exec_non_SELECT_Parametrized_Query(MYSQL *connection, const std::string &query, const std::vector<std::string> &parameters)
{ // Source: https://dev.mysql.com/doc/c-api/5.7/en/mysql-stmt-bind-param.html

    // Initialize a MySQL statement
    MYSQL_STMT *stmt = mysql_stmt_init(connection);

    if (!stmt)
    {
        printf("\nStatement initialization failed!\n");
        exit(1);
    }

    // Prepare the SQL statement
    if (mysql_stmt_prepare(stmt, query.c_str(), query.size()))
    {
        mysql_stmt_close(stmt);
        printf("\nStatement preparation failed!\n");
        exit(1);
    }

    // Bind parameters if there are any
    if (!parameters.empty())
    {
        MYSQL_BIND bind[parameters.size()]{};

        for (size_t i = 0; i < parameters.size(); ++i)
        {
            bind[i].buffer_type = MYSQL_TYPE_STRING;
            bind[i].buffer = (void *)parameters[i].c_str();
            bind[i].buffer_length = parameters[i].size();
        }

        if (mysql_stmt_bind_param(stmt, bind))
        {
            mysql_stmt_close(stmt);
            printf("\nParameter binding failed!\n");
            exit(1);
        }
    }

    // Execute the SQL statement
    if (mysql_stmt_execute(stmt))
    {
        mysql_stmt_close(stmt);
        printf("\nStatement execution failed: %s\n", mysql_stmt_error(stmt));
        exit(1);
    }

    //---------test---------
    // printf("\nExecuting query: %s\n", query.c_str());
    // printf("\nexec_non_SELECT_Parametrized_Query func! -> db succesfully modified\n");
    //--------------------------

    my_ulonglong affectedRows = mysql_stmt_affected_rows(stmt);
    printf("Number of affected rows: %llu\n", affectedRows);

    // Return a tuple containing the result set, number of rows, and number of fields
    // return std::make_tuple(result, mysql_num_rows(result), mysql_num_fields(result));
}
//=================----------------------------======================
auto exec_SQL_Query(MYSQL *connection, std::string query)
{
    if (mysql_query(connection, query.c_str()))
    {
        fprintf(stderr, "Query execution failed!\n");
        exit(1);
    }

    MYSQL_RES *result = mysql_store_result(connection);
    if (!result)
    {
        fprintf(stderr, "Query result failed!\n");
        exit(1);
    }

    // mysql_free_result(result);

    return std::make_tuple(result, mysql_num_rows(result), mysql_num_fields(result));
}

//=========================================================================================================================================

// class client_1_DB_History
// {
// public:
//     int idDonation[100];
//     // int idClient_0;
//     // int idClient_1;
//     // int idProduct;
//     // std::string type;
//     // int amount;
// private:
//     void mysql_store_result(void *arg)
//     {
//         struct thData tdL;
//         tdL = *((struct thData *)arg);

//         pthread_mutex_lock(&mutex);
//         printf("[Thread %d]The request connection from [client_1] has been received...\n\n", tdL.idThread);
//         pthread_mutex_unlock(&mutex);

//         SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");
//         auto result = exec_SQL_Query(SQLDetails.getConnection(), "SELECT * FROM FWR_DB.Donations WHERE ID0 IS NULL");

//         // Unpack the tupleD
//         MYSQL_RES *resultSet;
//         unsigned long numRows, numFields;
//         std::tie(resultSet, numRows, numFields) = result;

//         // Print the result
//         if (numRows == 0)
//         {
//             pthread_mutex_lock(&mutex);
//             std::string msg = "========> Unfortunately, the list of requests is empty...\n========> Please try again later!\n";
//             if (write(tdL.cl, msg.c_str(), msg.length()) <= 0) // 1.write_1.1
//             {
//                 perror("\n[Thread]Error at the write() function!\n");
//                 return;
//             }
//             pthread_mutex_unlock(&mutex);
//         }
//         else
//         {
//             printf("Number of Rows: %lu\n", numRows);
//             printf("Number of Fields: %lu\n", numFields);

//             MYSQL_ROW row;
//             while (row = mysql_fetch_row(resultSet))
//             {
//                 std::stringstream buffer;
//                 buffer << "ID_Donation: " << row[0] << " | ID1: " << row[2] << "\n";
//                 std::string result = buffer.str();

//                 pthread_mutex_lock(&mutex);
//                 write(tdL.cl, result.c_str(), result.size());
//                 pthread_mutex_unlock(&mutex);
//             }
//         }

//         pthread_mutex_lock(&mutex);
//         std::string end_msg = "end";
//         write(tdL.cl, end_msg.c_str(), end_msg.size()); // 1.write_end
//         pthread_mutex_unlock(&mutex);

//         mysql_free_result(resultSet);
//     }
// };

class user_data
{
private:
    std::string username;

public:
    void username_list_viewer(void *arg)
    {
        struct thData tdL;
        tdL = *((struct thData *)arg);

        pthread_mutex_lock(&mutex);
        printf("\n[Thread %d]The request to display the list of usernames from [client_0] has been received...\n", tdL.idThread);
        pthread_mutex_unlock(&mutex);

        SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");
        auto result = exec_SQL_Query(SQLDetails.getConnection(), "SELECT * FROM FWR_DB.client0");

        // Unpack the tuple
        MYSQL_RES *resultSet;
        unsigned long numRows, numFields;
        std::tie(resultSet, numRows, numFields) = result;

        // Print the result
        if (numRows == 0)
        {
            pthread_mutex_lock(&mutex);
            std::string msg = "========> Unfortunately, the list of usernames is empty...\n========> Please try again later!\n";
            if (write(tdL.cl, msg.c_str(), msg.length()) <= 0) // 1.write_1.1
            {
                perror("\n[Thread]Error at the write() function!\n");
                return;
            }
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            printf("\nHere we have the avalable usernames data:\n");
            printf("Number of Rows: %lu\n", numRows);
            printf("Number of Fields: %lu\n", numFields);

            MYSQL_ROW row;
            while (row = mysql_fetch_row(resultSet))
            {
                std::stringstream buffer;
                buffer << "ID0: " << row[0] << " | Username: " << row[1] << "\n";
                std::string result = buffer.str();

                //======TEST=====
                // pthread_mutex_lock(&mutex);
                // printf("\n TEST---: %s\n", buffer.str().c_str());
                // pthread_mutex_unlock(&mutex);
                //===============

                pthread_mutex_lock(&mutex);
                write(tdL.cl, result.c_str(), result.size());
                pthread_mutex_unlock(&mutex);
            }
        }

        pthread_mutex_lock(&mutex);
        std::string end_msg = "end";
        write(tdL.cl, end_msg.c_str(), end_msg.size()); // 1.write_end
        pthread_mutex_unlock(&mutex);

        mysql_free_result(resultSet);
    }

    void donation_list_viewer(void *arg)
    {
        // pthread_mutex_lock(&mutex);

        struct thData tdL;
        tdL = *((struct thData *)arg);

        pthread_mutex_lock(&mutex);
        printf("\n[Thread %d]The request to display the list of donations from [client_0] has been received...\n\n", tdL.idThread);
        pthread_mutex_unlock(&mutex);

        SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");
        auto result = exec_SQL_Query(SQLDetails.getConnection(), "SELECT * FROM FWR_DB.Products JOIN FWR_DB.Donations ON FWR_DB.Products.ID_Donation = FWR_DB.Donations.ID_Donation WHERE FWR_DB.Donations.ID0 IS NULL");

        // Unpack the tuple
        MYSQL_RES *resultSet;
        unsigned long numRows, numFields;
        std::tie(resultSet, numRows, numFields) = result;

        // Print the result
        if (numRows == 0)
        {
            OK = 0;

            pthread_mutex_lock(&mutex);
            std::string msg = "========> Unfortunately, the list of donations is empty...\n========> Please try again later!\n";
            if (write(tdL.cl, msg.c_str(), msg.length()) <= 0) // write_1.1
            {
                perror("\n[Thread]Error at the write() function!\n");
                return;
            }
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            OK = 1;

            pthread_mutex_lock(&mutex);
            printf("Here we have the avalable donations data:\n");
            printf("Number of Rows: %lu\n", numRows);
            printf("Number of Fields: %lu\n", numFields);
            pthread_mutex_unlock(&mutex);

            MYSQL_ROW row;
            while (row = mysql_fetch_row(resultSet))
            {
                std::stringstream buffer;
                buffer << "ID_Product: " << row[0] << " | ID_Donation: " << row[1] << " | Type: " << row[2] << " | Amount: " << row[3] << "\n";
                std::string result = buffer.str();

                pthread_mutex_lock(&mutex);
                write(tdL.cl, result.c_str(), result.size()); // write_1.2
                pthread_mutex_unlock(&mutex);
            }
        }

        pthread_mutex_lock(&mutex);
        std::string end_msg = "end";
        write(tdL.cl, end_msg.c_str(), end_msg.size()); // write_1_end
        pthread_mutex_unlock(&mutex);

        mysql_free_result(resultSet);
    }

    void store_current_name(void *arg)
    {
        struct thData tdL;
        tdL = *((struct thData *)arg);
        int test = 0;

        pthread_mutex_lock(&mutex);
        printf("\n[Thread %d]The request to store the current username selected from [client_0] has been received...\n\n", tdL.idThread);
        pthread_mutex_unlock(&mutex);

        read(tdL.cl, &this->username, sizeof(this->username)); // read_1.3 - username

        pthread_mutex_lock(&mutex);
        printf("Here we have the current username stored: %s\n", username.c_str());
        pthread_mutex_unlock(&mutex);

        //======TEST=====
        pthread_mutex_lock(&mutex);
        printf("\n TEST---: %s\n", username.c_str());
        pthread_mutex_unlock(&mutex);
        //===============

        write(tdL.cl, &test, sizeof(int)); // write_1.3 - test
    }
};

//========================================================================================================================================= main code

int main()
{
    system("clear");

    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr;            // mesajul primit de trimis la client
    int sd;            // descriptorul de socket
    pthread_t th[100]; // Identificatorii thread-urilor care se vor crea
    int i = -1;

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

    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; // parametru functia executata de thread
        unsigned int length = sizeof(from);

        printf("\n-----------------------------------------\n");

        printf("\n[server]I'm waiting at the port %d...\n", PORT);
        fflush(stdout);

        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("\n[server]Error at the accept() function!\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */
        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = ++i;
        td->cl = client;

        struct thData tdL;
        tdL = *((struct thData *)td);

        pthread_mutex_lock(&mutex);
        if (read(tdL.cl, &nr, sizeof(int)) <= 0) // read_1
        {
            printf("\n[Thread %d]\n", tdL.idThread);
            perror("Error at the read() function!\n");
        }
        pthread_mutex_unlock(&mutex);

        if (nr == 0)
            pthread_create(&th[i], NULL, &treat_client_0, td);
        else
            pthread_create(&th[i], NULL, &treat_client_1, td);

        // system("clear");
    }
};

//========================================================================================================================================= client_0 code

static void *treat_client_0(void *arg)
{
    struct thData *tdL = (struct thData *)arg;
    fflush(stdout);
    pthread_detach(pthread_self());
    user_data user;
    user.username_list_viewer((struct thData *)arg);
    //------------------------------------------
    user.store_current_name((struct thData *)arg);
    //------------------------------------------
    user.donation_list_viewer((struct thData *)arg);
    //------------------------------------------
    if (OK)
        request_sent((struct thData *)arg);
    //------------------------------------------
    final_message((struct thData *)arg);

    /* am terminat cu acest client, inchidem conexiunea */
    free(tdL);
    close((intptr_t)arg);
    return (NULL);
};

void request_sent(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    int donation_value;

    if (read(tdL.cl, &donation_value, sizeof(int)) <= 0) // read_2 - get the donation_value selected
    {
        printf("[Thread %d]\n", tdL.idThread);
        perror("Error at the read() function!\n\n");
    }

    //----test-----------
    // printf("\n TEST\n");
    // printf("donation_value: %d\n", donation_value);
    //--------------------

    pthread_mutex_lock(&mutex);
    printf("\n[Thread %d]The request donation from [client_0] has been received...\n", tdL.idThread);
    pthread_mutex_unlock(&mutex);

    SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");
    std::ostringstream queryStream_2; // Create a parameterized query
    queryStream_2 << "SELECT * FROM FWR_DB.Products p JOIN FWR_DB.Donations d ON p.ID_Donation = d.ID_Donation WHERE d.ID_Donation = " << donation_value << " AND d.ID0 IS NULL";
    // std::vector<std::string> parameters_2 = {std::to_string(donation_value)};
    // auto result_2 = exec_non_SELECT_Parametrized_Query(SQLDetails.getConnection(), queryStream_2.str(), parameters_2);
    auto result_2 = exec_SQL_Query(SQLDetails.getConnection(), queryStream_2.str());
    // Unpack the tuple
    MYSQL_RES *resultSet_2;
    unsigned long numRows_2, numFields_2;
    std::tie(resultSet_2, numRows_2, numFields_2) = result_2;

    //----------test------------
    // printf("\n\n numRows-TEST: %ld\n\n", numRows_2);
    //---------------------------

    // Print the result
    if (numRows_2 == 0)
    {
        std::string msg = "Unfortunately, there is no donation with the value selected...";

        //--------test-----------
        // printf("\n\n msg-TEST: %s\n\n", msg.c_str());
        //-----------------------

        pthread_mutex_lock(&mutex);
        if (write(tdL.cl, msg.c_str(), msg.length()) <= 0) // write_2.1
        {
            perror("[Thread]Error at the write() function!\n\n");
            return;
        }
        pthread_mutex_unlock(&mutex);
    }
    else
    {
        pthread_mutex_lock(&mutex);
        printf("Here we have the selected donation data:\n");
        printf("Number of Rows (selected donation): %lu\n", numRows_2);
        printf("Number of Fields (selected donation): %lu\n", numFields_2);
        pthread_mutex_unlock(&mutex);

        MYSQL_ROW row;
        while (row = mysql_fetch_row(resultSet_2))
        {
            std::stringstream buffer_2;
            buffer_2 << "ID_Product: " << row[0] << " | ID_Donation: " << row[1] << " | Type: " << row[2] << " | Amount: " << row[3] << "\n";
            std::string result_2 = buffer_2.str();

            //--------test-----------
            // printf("\n\n result_2-TEST: %s", result_2.c_str());
            //-----------------------

            pthread_mutex_lock(&mutex);
            write(tdL.cl, result_2.c_str(), result_2.size()); // write_2.2
            pthread_mutex_unlock(&mutex);
        }

        sleep(1); // HEAVEN

        pthread_mutex_lock(&mutex);
        std::string end_msg = "end";
        write(tdL.cl, end_msg.c_str(), end_msg.size()); // write_2_end
        pthread_mutex_unlock(&mutex);
    }
    mysql_free_result(resultSet_2);

    //--------------------------------------

    // std::ostringstream queryStream_3; // Create a parameterized query
    // queryStream_3 << "UPDATE FWR_DB.Donations SET FWR_DB.Donations.ID0 = " << tdL.idThread << " "
    //               << "WHERE FWR_DB.Donations.ID_Donation = " << donation_value;
    // auto result_3 = exec_SQL_Query(SQLDetails.getConnection(), queryStream_3.str());

    std::string queryStream_3 = "UPDATE FWR_DB.Donations SET FWR_DB.Donations.ID0 = ? WHERE FWR_DB.Donations.ID_Donation = ?";
    int id_value = tdL.idThread + 1;
    std::vector<std::string> parameters = {std::to_string(id_value), std::to_string(donation_value)};

    //-------test----------
    // pthread_mutex_lock(&mutex);
    // printf("\n TEST\n");
    // printf("idThread: %d\n", id_value);
    // printf("donation_value: %d\n", donation_value);
    // pthread_mutex_unlock(&mutex);
    //---------------------

    try
    {
        // auto result_3 = exec_non_SELECT_Parametrized_Query(SQLDetails.getConnection(), queryStream_3, parameters);

        // Extract information from the result tuple
        // MYSQL_RES *result = std::get<0>(result_3);
        // int numRows = std::get<1>(result_3);
        // int numFields = std::get<2>(result_3);

        // auto result_3 = exec_SQL_Query(SQLDetails.getConnection(), queryStream_3.str());

        // Unpack the tuple
        // MYSQL_RES *resultSet;
        // unsigned long numRows, numFields;
        // std::tie(resultSet, numRows, numFields) = result_3;

        //------------test----------
        // pthread_mutex_lock(&mutex);
        // printf("\n TEST\n");
        // printf("\n %ld\n", numRows);
        // printf("\n %ld\n", numFields);
        // pthread_mutex_unlock(&mutex);
        //--------------------------

        pthread_mutex_lock(&mutex);
        printf("\nUPDATE query will notify the client_1 with the request donation!\n");
        exec_non_SELECT_Parametrized_Query(SQLDetails.getConnection(), queryStream_3, parameters);
        pthread_mutex_unlock(&mutex);

        // printf("Query executed successfully. Rows affected: %ld\n", numRows);
        // printf("Query executed successfully. Fields affected: %ld\n", numFields);

        // Clean up resources
        // mysql_free_result(resultSet);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

//========================================================================================================================================= client_1 code
//=========================================================================================================================================

static void *treat_client_1(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    fflush(stdout);
    pthread_detach(pthread_self());
    request_list_viewer((struct thData *)arg);
    //------------------------------------------
    final_message((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    return (NULL);
};

void request_list_viewer(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);

    pthread_mutex_lock(&mutex);
    printf("[Thread %d]The request connection from [client_1] has been received...\n\n", tdL.idThread);
    pthread_mutex_unlock(&mutex);

    SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");
    auto result = exec_SQL_Query(SQLDetails.getConnection(), "SELECT * FROM FWR_DB.Donations WHERE ID0 IS NULL");

    // Unpack the tupleD
    MYSQL_RES *resultSet;
    unsigned long numRows, numFields;
    std::tie(resultSet, numRows, numFields) = result;

    // Print the result
    if (numRows == 0)
    {
        pthread_mutex_lock(&mutex);
        std::string msg = "========> Unfortunately, the list of requests is empty...\n========> Please try again later!\n";
        if (write(tdL.cl, msg.c_str(), msg.length()) <= 0) // 1.write_1.1
        {
            perror("\n[Thread]Error at the write() function!\n");
            return;
        }
        pthread_mutex_unlock(&mutex);
    }
    else
    {
        printf("Number of Rows: %lu\n", numRows);
        printf("Number of Fields: %lu\n", numFields);

        MYSQL_ROW row;
        while (row = mysql_fetch_row(resultSet))
        {
            std::stringstream buffer;
            buffer << "ID_Donation: " << row[0] << " | ID1: " << row[2] << "\n";
            std::string result = buffer.str();

            pthread_mutex_lock(&mutex);
            write(tdL.cl, result.c_str(), result.size());
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_mutex_lock(&mutex);
    std::string end_msg = "end";
    write(tdL.cl, end_msg.c_str(), end_msg.size()); // 1.write_end
    pthread_mutex_unlock(&mutex);

    mysql_free_result(resultSet);
}

//--------------------------------------------------------------

void final_message(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);

    pthread_mutex_lock(&mutex);
    printf("\n[Thread %d]The current thread connection has been finished!\n\n", tdL.idThread);
    pthread_mutex_unlock(&mutex);
}

//=========================================MariaDB=======================================================================================

/* EXAMPLE OF QUERY

    SQLConnection SQLDetails("localhost", "admin_user", "Lrt54%hh", "FWR_DB");

    auto result=exec_SQL_Query(SQLDetails.getConnection(), "SELECT * FROM Donations");

    // Unpack the tuple
    MYSQL_RES *resultSet;
    unsigned long numRows, numFields;
    std::tie(resultSet, numRows, numFields) = result;

    // Print the result
    printf("Number of Rows: %lu\n", numRows);
    printf("Number of Fields: %lu\n", numFields);

    MYSQL_ROW row;
    while(row = mysql_fetch_row(resultSet))
    {
        printf("ID_Donation: %s | ID0: %s | ID1: %s\n", row[0], row[1], row[2]);
    }

    mysql_free_result(resultSet);
*/

//====================================================================================================================== second main code
