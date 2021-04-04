#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include "my_time.h"


#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) {
    int parentfd; /* parent socket */
    int childfd; /* child socket */
    int portno; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    char buf[BUFSIZE]; /* message buffer */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    int connectcnt; /* number of connection requests */
    int notdone;
    fd_set readfds;

    /*
     * check command line arguments
     */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /*
     * socket: create the parent socket
     */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0)
        error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval , sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short)portno);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        error("ERROR on binding");

    /*
     * listen: make this socket ready to accept connection requests
     */
    if (listen(parentfd, 10000) < 0) /* allow 5 requests to queue up */
        error("ERROR on listen");


    /* initialize some things for the main loop */
    clientlen = sizeof(clientaddr);
    notdone = 1;
    connectcnt = 0;
//    printf("server> ");

    /*
     * If connection request, then echo input line
     * and close connection.
     */
    auto start = get_current_time();
    std::ofstream f{"../result.txt"};
    while (notdone) {

        /*
         * select: Has the user typed something to stdin or
         * has a connection request arrived?
         */
        FD_ZERO(&readfds);          /* initialize the fd set */
        FD_SET(parentfd, &readfds); /* add socket fd */
//        FD_SET(0, &readfds);        /* add stdin fd (0) */
        if (select(parentfd+1, &readfds, 0, 0, 0) < 0) {
            error("ERROR in select");
        }
        std::cout << "SELECT DONE!\n";

        /* if a connection request has arrived, process it */
        if (FD_ISSET(parentfd, &readfds)) {
            /*
             * accept: wait for a connection request
             */
            childfd = accept(parentfd,(struct sockaddr *) &clientaddr, &clientlen);
            if (childfd < 0)
                error("ERROR on accept");
            connectcnt++;

            std::cout << "ACCEPT DONE!\n";

            /*
             * read: read input string from the client
             */
            bzero(buf, BUFSIZE);
            n = read(childfd, buf, BUFSIZE);

            std::cout << "READ` DONE!\n";
            if (n < 0)
                error("ERROR reading from socket");

            /*
             * write: echo the input string back to the client
             */
            n = write(childfd, buf, strlen(buf));
            if (n < 0)
                error("ERROR writing to socket");

            close(childfd);
        }

        if (to_us(get_current_time() - start) > 1000000) {
            f.close();
            f = std::ofstream {"../result.txt"};
            f << connectcnt << std::endl;
            start = get_current_time();
//            v_conn_per_sec.push_back(alive_connections);
            std::cout << connectcnt << std::endl;
            connectcnt = 0;
        }
    }

    /* clean up */
//    printf("Terminating server.\n");
    close(parentfd);
    exit(0);
}