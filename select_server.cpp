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
#include <vector>
#include <algorithm>
#include <set>
#include "Selector.h"


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
               (const void *) &optval, sizeof(int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short) portno);

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
    std::set<int> descriptors_alive;
    Selector selector{};
    fd_set master_set;
    fd_set writefds;
    fd_set readfds;
    fd_set accept_set;
    int max_fd = parentfd;
    FD_SET(parentfd, &master_set);
    descriptors_alive.insert(parentfd);
    std::ofstream f{"../result.txt"};
    while (notdone) {
        FD_ZERO(&accept_set);          /* initialize the fd set */
        FD_SET(parentfd, &accept_set); /* add socket fd */
        struct timeval t{0, 1};
        int threshold = parentfd < selector.get_max_descriptor() ? selector.get_max_descriptor(): parentfd;
        if (select(threshold + 1, &accept_set, 0, 0, &t) < 0) { // trickky shit with timer
            error("ERROR in select");
        }

        /* if a connection request has arrived, process it */
        if (FD_ISSET(parentfd, &accept_set)) {
            childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
            selector.register_read_fd(childfd);
            connectcnt++;
        }

        for (auto i: selector.extract_readables(1)) {
            bzero(buf, BUFSIZE);
            n = read(i, buf, BUFSIZE);
            if (n < 0)
                error("ERROR reading from socket");
            selector.register_write_fd(i);
        }

        for (auto i: selector.extract_writeables(1)) {
            n = write(i, buf, strlen(buf));
            if (n < 0)
                error("ERROR writing to socket");
            close(i);
        }

        if (to_us(get_current_time() - start) > 1000000) {
            f.close();
            f = std::ofstream{"../result.txt"};
            f << connectcnt << std::endl;
            start = get_current_time();
//            v_conn_per_sec.push_back(alive_connections);
            std::cout << "Alive connections: " << connectcnt << std::endl;
            connectcnt = 0;
        }
    }
    /* clean up */
    //    printf("Terminating server.\n");
    close(parentfd);
    exit(0);
}
