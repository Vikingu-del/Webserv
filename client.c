/* 
 *       flight-time-client.c : get flight time from the server
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#define FLIGHT_NUM_SIZE            15

#define SERVER_PORT                "4358"
#define STORE_FLIGHT               1
#define FLIGHT_TIME_STORED         2
#define FLIGHT_TIME                3
#define FLIGHT_TIME_RESULT         4
#define FLIGHT_NOT_FOUND           5
#define ERROR_IN_INPUT             9
#define QUIT                       0

void error (char *msg);

struct message {
    int32_t message_id;
    char flight_no [FLIGHT_NUM_SIZE + 1];
    char departure [1 + 1]; // 'D': departure, 'A': arrival
    char date [10 + 1]; // dd/mm/yyyy
    char time [5 + 1];   // hh:mm
};

struct message message;

int get_input (void);
void error (char *msg);

int main (int argc, char **argv)
{
    if (argc != 2) {
        fprintf (stderr, "Usage: client hostname\n");
        exit (EXIT_FAILURE);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *result;
    int s; 
    if ((s = getaddrinfo (argv [1], SERVER_PORT, &hints, &result)) != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        exit (EXIT_FAILURE);
    }

    /* Scan through the list of address structures returned by 
       getaddrinfo. Stop when the the socket and connect calls are successful. */

    int sock_fd;
    socklen_t length;
    struct addrinfo *rptr;
    for (rptr = result; rptr != NULL; rptr = rptr -> ai_next) {
        sock_fd = socket (rptr -> ai_family, rptr -> ai_socktype,
                       rptr -> ai_protocol);
        if (sock_fd == -1)
            continue;

        if (connect (sock_fd, rptr -> ai_addr, rptr -> ai_addrlen) == -1) {
            if (close (sock_fd) == -1)
                error ("close");
            continue;
        }
        
        break;
    }

    if (rptr == NULL) {               // Not successful with any address
        fprintf(stderr, "Not able to connect\n");
        exit (EXIT_FAILURE);
    }

    freeaddrinfo (result);

    int option;

    while (1) {
         option = get_input ();
         if (option == QUIT)
             break;

         // send request to server
         if (send (sock_fd, &message, sizeof (struct message), MSG_NOSIGNAL) == -1)
             error ("send");

         // receive response from server
         if (recv (sock_fd, &message, sizeof (struct message), 0) == -1)
             error ("recv");

         // process server response 
         switch (ntohl (message.message_id)) {
             case FLIGHT_TIME_STORED: 
             case FLIGHT_TIME_RESULT: printf ("\nResponse: \n\n");
                    printf ("\t%s: %s %s %s\n\n", message.flight_no, message.departure, 
                                              message.date, message.time);
                     break;
             case FLIGHT_NOT_FOUND: printf ("\nFlight not found\n\n");
                     break;
             case ERROR_IN_INPUT: printf ("\nError in input\n\n");
                     break;
             default: printf ("\nUnrecongnized message from server\n\n");
         }
    }

    exit (EXIT_SUCCESS);
}

char inbuf [512];

int get_input (void)
{
    int option;

    while (1) {
        printf ("Flight Info\n\n");
        printf ("\tFlight time query\t1\n");
        printf ("\tStore flight time\t2\n");
        printf ("\tQuit\t\t\t0\n\n");
        printf ("Your option: ");
        if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
            error ("fgets");
        sscanf (inbuf, "%d", &option);

        int len;

        switch (option) {

            case 1: message.message_id = htonl (FLIGHT_TIME);
                    printf ("Flight no: ");
                    if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
                        error ("fgets");
                    len = strlen (inbuf);
                    if (inbuf [len - 1] == '\n')
                        inbuf [len - 1] = '\0';
                    strcpy (message.flight_no, inbuf);
                    break;

            case 2: message.message_id = htonl (STORE_FLIGHT);
                    printf ("Flight no: ");
                    if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
                        error ("fgets");
                    len = strlen (inbuf);
                    if (inbuf [len - 1] == '\n')
                        inbuf [len - 1] = '\0';
                    strcpy (message.flight_no, inbuf);

                    while (1) {
                        printf ("A/D: ");
                        if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
                            error ("fgets");
                        message.departure [0] = toupper (inbuf [0]);
                        message.departure [1] = '\0';
                        if ((message.departure [0] == 'A') || (message.departure [0] == 'D'))
                            break;
                        printf ("Error in input, valid values are A and D\n");
                    }     
                    while (1) {
                        printf ("date (dd/mm/yyyy): ");
                        if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
                            error ("fgets");
                        strncpy (message.date, inbuf, 10);
                        message.date [10] = '\0';
                        if (strchr (message.date, '/') != NULL)
                            break;
                        printf ("Error in input, date should be in dd/mm/yyyy format\n");
                    }
                    while (1) {
                        printf ("time (hh:mm): ");
                        if (fgets (inbuf, sizeof (inbuf),  stdin) == NULL)
                            error ("fgets");
                        strncpy (message.time, inbuf, 5);
                        message.time [5] = '\0';
                        if (strchr (message.time, ':') != NULL)
                            break;
                        printf ("Error in input, time should be in hh:mm format\n");
                    }
                    break;

            case 0:
                    break;

            default: printf ("Illegal option, try again\n\n");
                     continue;

        }

        return option;
    }
}

void error (char *msg)
{
    perror (msg);
    exit (1);
}