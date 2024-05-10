#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <arpa/inet.h>

#define SERVERPORT 8080
#define SERVER_BACKLOG 5
#define BUFSIZE 1024
#define SOCKETERROR -1

typedef struct sockaddr SA;
typedef struct sockaddr_in SA_IN;


int check(int exp, const char *msg)
{
    if (exp = SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return exp;
}

int setup_server(short port, int backlog) {
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create socket");

    //initialize the address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)), "Failed to bind socket");
    check(listen(server_socket, backlog), "Failed to listen on socket");
    return server_socket;
}

int accept_new_connection(int server_socket) {
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;
    check(client_socket = accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size), "Failed to accept connection");
    return client_socket;
}

void *handle_connection(int client_socket) {
    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actualpath[PATH_MAX + 1];

    //read the client's message -- the name of the file to read
    while((bytes_read = read(client_socket, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0) {
        msgsize += bytes_read;
        if (msgsize > BUFSIZE - 1 || buffer[msgsize - 1] == '\n') break;
    }
    check(bytes_read, "recv error");
    buffer[msgsize - 1] = 0; //null terminate the message
    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    //validity check: make sure the path is under the current directory
    if (realpath(buffer, actualpath) == NULL) {
        printf("ERROR(bad path): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    //read file and send its contents to client
    FILE *fp = fopen(actualpath, "r");
    if (fp == NULL) {
        printf("ERROR(open): %s\n", buffer);
        close(client_socket);
        return NULL;
    }

    //read file contents and send them to client
    //note this is a fine example program, but rather insecure.
    //a real program would probably limit the client to certain files, and might use chroot() to sandbox the process
    while ((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0) {
        //printf("sending %zu bytes\n", bytes_read);
        write(client_socket, buffer, bytes_read);
    }
    close(client_socket);
    fclose(fp);
    printf("Closing connection\n");
    return NULL;
}

int main(int argc, char **argv)
{
    int server_socket = setup_server(SERVERPORT, SERVER_BACKLOG);

    fd_set current_sockets, ready_sockets;

    //initialize my current set
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);

    while (1) {
        //because select is destructive, we need to reset ready_sockets every time
        ready_sockets = current_sockets;
        
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == server_socket) {
                    //accept new connection
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                } else {
                    //handle the connection
                    handle_connection(i);
                    close(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }
    } //while

    return EXIT_SUCCESS;
}

