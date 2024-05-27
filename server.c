/* 
 *           flight-time-server.c: record and provide time of a
 *                                 flight from the airport
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
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

#define BACKLOG                   10
#define NUM_FDS                    5

void error (char *msg);

struct message {
    int32_t message_id;
    char flight_no [FLIGHT_NUM_SIZE + 1];
    char departure [1 + 1]; // 'D': departure, 'A': arrival
    char date [10 + 1]; // dd/mm/yyyy
    char time [5 + 1];   // hh:mm
};

struct tnode {
    char *flight_no;
    bool departure; // true: departure, false: arrival
    time_t flight_time;
    struct tnode *left;
    struct tnode *right;
};

struct message recv_message, send_message;

struct tnode *add_to_tree (struct tnode *p, char *flight_no, bool departure, time_t flight_time);
struct tnode *find_flight_rec (struct tnode *p, char *flight_no);
void print_tree (struct tnode *p);
void trim (char *dest, char *src); 
void error (char *msg);

int main (int argc, char **argv)
{
    const char * const ident = "flight-time-server";

    openlog (ident, LOG_CONS | LOG_PID | LOG_PERROR, LOG_USER);
    syslog (LOG_USER | LOG_INFO, "%s", "Hello world!");
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_PASSIVE;    /* for wildcard IP address */

    struct addrinfo *result;
    int s; 
    if ((s = getaddrinfo (NULL, SERVER_PORT, &hints, &result)) != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        exit (EXIT_FAILURE);
    }

    /* Scan through the list of address structures returned by 
       getaddrinfo. Stop when the the socket and bind calls are successful. */

    int listener, optval = 1;
    socklen_t length;
    struct addrinfo *rptr;
    for (rptr = result; rptr != NULL; rptr = rptr -> ai_next) {
        listener = socket (rptr -> ai_family, rptr -> ai_socktype,
                       rptr -> ai_protocol);
        if (listener == -1)
            continue;

        if (setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
            error("setsockopt");

        if (bind (listener, rptr -> ai_addr, rptr -> ai_addrlen) == 0)  // Success
            break;

        if (close (listener) == -1)
            error ("close");
    }

    if (rptr == NULL) {               // Not successful with any address
        fprintf(stderr, "Not able to bind\n");
        exit (EXIT_FAILURE);
    }

    freeaddrinfo (result);

    // Mark socket for accepting incoming connections using accept
    if (listen (listener, BACKLOG) == -1)
        error ("listen");

    nfds_t nfds = 0;
    struct pollfd *pollfds;
    int maxfds = 0, numfds = 0;

    if ((pollfds = malloc (NUM_FDS * sizeof (struct pollfd))) == NULL)
	error ("malloc");
    maxfds = NUM_FDS;

    pollfds -> fd = listener;
    pollfds -> events = POLLIN;
    pollfds -> revents = 0;
    numfds = 1;

    socklen_t addrlen;
    struct sockaddr_storage client_saddr;
    char str [INET6_ADDRSTRLEN];
    struct sockaddr_in  *ptr;
    struct sockaddr_in6  *ptr1;
    struct tnode *root = NULL;

    while (1) {
        // monitor readfds for readiness for reading
	nfds = numfds;
	if (poll (pollfds, nfds, -1) == -1)
	    error ("poll");
        
        // Some sockets are ready. Examine readfds
        for (int fd = 0; fd < (nfds + 1); fd++) {
            if ((pollfds + fd) -> fd <= 0) // file desc == 0 is not expected, as these are socket fds and not stdin
		continue;

            if (((pollfds + fd) -> revents & POLLIN) == POLLIN) {  // fd is ready for reading 
                if ((pollfds + fd) -> fd == listener) {  // request for new connection
                    addrlen = sizeof (struct sockaddr_storage);
                    int fd_new;
                    if ((fd_new = accept (listener, (struct sockaddr *) &client_saddr, &addrlen)) == -1)
                        error ("accept");
                    // add fd_new to pollfds
		    if (numfds == maxfds) { // create space
                        if ((pollfds = realloc (pollfds, (maxfds + NUM_FDS) * sizeof (struct pollfd))) == NULL)
	                    error ("malloc");
                        maxfds += NUM_FDS;
		    }
                    numfds++;
		    (pollfds + numfds - 1) -> fd = fd_new;
                    (pollfds + numfds - 1) -> events = POLLIN;
                    (pollfds + numfds - 1) -> revents = 0;

                    // print IP address of the new client
                    if (client_saddr.ss_family == AF_INET) {
                        ptr = (struct sockaddr_in *) &client_saddr;
                        inet_ntop (AF_INET, &(ptr -> sin_addr), str, sizeof (str));
                    }
                    else if (client_saddr.ss_family == AF_INET6) {
                        ptr1 = (struct sockaddr_in6 *) &client_saddr;
	                inet_ntop (AF_INET6, &(ptr1 -> sin6_addr), str, sizeof (str));
                    }
                    else
                    {
                        ptr = NULL;
                        fprintf (stderr, "Address family is neither AF_INET nor AF_INET6\n");
                    }
                    if (ptr) 
                        syslog (LOG_USER | LOG_INFO, "%s %s", "Connection from client", str);
                
                }
                else  // data from an existing connection, receive it
                {
                    memset (&recv_message, '\0', sizeof (struct message));
                    ssize_t numbytes = recv ((pollfds + fd) -> fd, &recv_message, sizeof (struct message), 0);
   
                    if (numbytes == -1)
                        error ("recv");
                    else if (numbytes == 0) {
                        // connection closed by client
                        fprintf (stderr, "Socket %d closed by client\n", (pollfds + fd) -> fd);
                        if (close ((pollfds + fd) -> fd) == -1)
                            error ("close");
			(pollfds + fd) -> fd *= -1; // make it negative so that it is ignored in future
                    }
                    else 
                    {
                        // data from client
                        bool valid;
                        char temp_buf [FLIGHT_NUM_SIZE + 1];
                        
                        switch (ntohl (recv_message.message_id)) {
                            case STORE_FLIGHT:
                                   valid = true;
                                   // validate flight number
                                   if (recv_message.flight_no [FLIGHT_NUM_SIZE])
                                       recv_message.flight_no [FLIGHT_NUM_SIZE] = '\0';
                                   if (strlen (recv_message.flight_no) < 3)
                                       valid = false;
                                   trim (temp_buf, recv_message.flight_no);
                                   strcpy (recv_message.flight_no, temp_buf);
                                   bool departure;
                                   if (toupper (recv_message.departure [0]) == 'D')
                                       departure = true;
                                   else if (toupper (recv_message.departure [0]) == 'A')
                                       departure = false; 
                                   else
                                       valid = false;

                                   char delim [] = "/";
                                   char *mday, *month, *year, *saveptr;
                                   mday = month = year = NULL;
                                   mday = strtok_r (recv_message.date, delim, &saveptr);
                                   if (mday)
                                       month = strtok_r (NULL, delim, &saveptr);
                                   else 
                                       valid = false;
                                   if (month)
                                       year = strtok_r (NULL, delim, &saveptr);
                                   else 
                                       valid = false;
                                   if (!year)
                                       valid = false;
                                   char *hrs, *min;
                                   // get time
                                   if (recv_message.time [5])
                                       recv_message.time [5] = '\0';
                                   delim [0] = ':';
                                   hrs = min = NULL;
                                   hrs = strtok_r (recv_message.time, delim, &saveptr);
                                   if (hrs) 
                                       min = strtok_r (NULL, delim, &saveptr);
                                   if (!hrs || !min)
                                       valid = false;

                                   time_t ts;

                                   if (valid) {
                                       struct tm tm;

                                       tm.tm_sec = 0;
                                       sscanf (min, "%d", &tm.tm_min);
                                       sscanf (hrs, "%d", &tm.tm_hour);
                                       sscanf (mday, "%d", &tm.tm_mday);
                                       sscanf (month, "%d", &tm.tm_mon);
                                       (tm.tm_mon)--;
                                       sscanf (year, "%d", &tm.tm_year);
                                       tm.tm_year -= 1900;
                                       tm.tm_isdst = -1;

                                       if ((ts = mktime (&tm)) == (time_t) -1)
                                           valid = false;
                                  
                                       time_t now;

                                       if ((now = time (NULL)) == (time_t) -1)
                                           error ("time");

                                       if (ts < now)
                                           valid = false;
                                   }

                                   if (!valid) {
                                       // send error message to client
                                       send_message.message_id = htonl (ERROR_IN_INPUT);
                                       size_t msg_len = sizeof (long);
                                       if (send ((pollfds + fd) -> fd, &send_message, msg_len, 0) == -1)
                                           error ("send");
                                   }
                                   else
                                   {
                                       // add flight data to tree
                                       root = add_to_tree (root, recv_message.flight_no, departure, ts);
                                       // send confirmation to client
                                       send_message.message_id = htonl (FLIGHT_TIME_STORED);
                                       strcpy (send_message.flight_no, recv_message.flight_no);
                                       strcpy (send_message.departure, (departure) ? "D" : "A");
                                       struct tm *tms;  
                                       if ((tms = localtime (&ts)) == NULL)  
                                            perror ("localtime");                    
                                       sprintf (send_message.date, "%02d/%02d/%d", tms -> tm_mday, 
                                            tms -> tm_mon + 1, tms -> tm_year + 1900);
                                       sprintf (send_message.time, "%02d:%02d", tms -> tm_hour,
                                            tms -> tm_min);
                                       size_t msg_len = sizeof (struct message);
                                       if (send ((pollfds + fd) -> fd, &send_message, msg_len, 0) == -1)
                                           error ("send");
                                   }
                                   break;
                            case FLIGHT_TIME:
                                   valid = true;
                                   // validate flight number
                                   if (recv_message.flight_no [FLIGHT_NUM_SIZE])
                                       recv_message.flight_no [FLIGHT_NUM_SIZE] = '\0';
                                   if (strlen (recv_message.flight_no) < 3)
                                       valid = false;
                                   if (!valid) {
                                       // send error message to client
                                       send_message.message_id = htonl (ERROR_IN_INPUT);
                                       size_t msg_len = sizeof (long);
                                       if (send ((pollfds + fd) -> fd, &send_message, msg_len, 0) == -1)
                                           error ("send");
                                       break;
                                   }
                                   char temp_buf [FLIGHT_NUM_SIZE + 1];
                                   trim (temp_buf, recv_message.flight_no);
                                   strcpy (recv_message.flight_no, temp_buf);
                                   struct tnode *ptr;
                                   ptr = find_flight_rec (root, recv_message.flight_no);
                                   if (!ptr) {
                                       memset (&send_message, '\0', sizeof (struct message));
                                       send_message.message_id = htonl (FLIGHT_NOT_FOUND);
                                       strcpy (send_message.flight_no, recv_message.flight_no);
                                       size_t msg_len = sizeof (struct message);
                                       if (send ((pollfds + fd) -> fd, &send_message, msg_len, 0) == -1)
                                           error ("send");
                                       break;
                                   }
                                   send_message.message_id = htonl (FLIGHT_TIME_RESULT);
                                   strcpy (send_message.flight_no, recv_message.flight_no);
                                   strcpy (send_message.departure, (ptr -> departure) ? "D" : "A");
                                   struct tm *tms;  
                                   if ((tms = localtime (&(ptr -> flight_time))) == NULL)  
                                        perror ("localtime");                    
                                   sprintf (send_message.date, "%02d/%02d/%d", tms -> tm_mday, 
                                            tms -> tm_mon + 1, tms -> tm_year + 1900);
                                   sprintf (send_message.time, "%02d:%02d", tms -> tm_hour,
                                            tms -> tm_min);
                                   size_t msg_len = sizeof (struct message);
                                   if (send ((pollfds + fd) -> fd, &send_message, msg_len, 0) == -1)
                                       error ("send");
                                   break;

                        }

                    }
                }
            } // if (fd == ...
        } // for
    } // while (1)

    exit (EXIT_SUCCESS);
} // main

// record the flight departure / arrival time    
struct tnode *add_to_tree (struct tnode *p, char *flight_no, bool departure, time_t flight_time)
{
    int res;

    if (p == NULL) {  // new entry
        if ((p = (struct tnode *) malloc (sizeof (struct tnode))) == NULL)
            error ("malloc");
        p -> flight_no = strdup (flight_no);
        p -> departure = departure;
        p -> flight_time = flight_time;
        p -> left = p -> right = NULL;
    }
    else if ((res = strcmp (flight_no, p -> flight_no)) == 0) { // entry exists
        p -> departure = departure;
        p -> flight_time = flight_time;
    }
    else if (res < 0) // less than flight_no for this node, put in left subtree
        p -> left = add_to_tree (p -> left, flight_no, departure, flight_time);
    else   // greater than flight_no for this node, put in right subtree
        p -> right = add_to_tree (p -> right, flight_no, departure, flight_time);
    return p;
}

// find node for the flight for which departure or arrival time is queried
struct tnode *find_flight_rec (struct tnode *p, char *flight_no)
{
    int res;

    if (!p) 
        return p;
    res = strcmp (flight_no, p -> flight_no);
    
    if (!res)
        return p;

    if (res < 0)
        return find_flight_rec (p -> left, flight_no);
    else 
        return find_flight_rec (p -> right, flight_no);
}

// print_tree: print the tree (in-order traversal)
void print_tree (struct tnode *p)
{
    if (p != NULL) {
        print_tree (p -> left);
        printf ("%s: %d %s\n\n", p -> flight_no, (int) p -> departure, ctime (&(p -> flight_time)));
        print_tree (p -> right);
    }
}

void error (char *msg)
{
    perror (msg);
    exit (1);
}

// trim: leading and trailing whitespace of string
void trim (char *dest, char *src)
{
    if (!src || !dest)
       return;

    int len = strlen (src);

    if (!len) {
        *dest = '\0';
        return;
    }
    char *ptr = src + len - 1;

    // remove trailing whitespace
    while (ptr > src) {
        if (!isspace (*ptr))
            break;
        ptr--;
    }

    ptr++;

    char *q;
    // remove leading whitespace
    for (q = src; (q < ptr && isspace (*q)); q++)
        ;

    while (q < ptr)
        *dest++ = *q++;

    *dest = '\0';
}