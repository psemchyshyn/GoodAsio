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
#include "../inc/my_time.h"
#include <vector>
#include <algorithm>
#include <set>
#include "../inc/Selector.h"
#include <thread>
#include <mutex>
#include <atomic>


#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}


void acceptoring() {
    int childfd;
    int parentfd; /* parent socket */
    int portno; /* port to listen on */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    char buf[BUFSIZE]; /* message buffer */
    int optval; /* flag value for setsockopt */
    int n; /* message byte size */
    int notdone;
    int loop = 1;
    int cnt;
    fd_set readfds;

    /*
     * check command line arguments
     */

    portno = 1024;

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
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEPORT,
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
    auto start = get_current_time();
    while (1) {
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0)
            error("ERROR on accept");
        cnt++;

        /*
         * read: read input string from the client
         */
        bzero(buf, BUFSIZE);
        int n = read(childfd, buf, BUFSIZE);
        if (n < 0)
            error("ERROR reading from socket");

        /*
         * write: echo the input string back to the client
         */
        n = write(childfd, buf, strlen(buf));
        if (n < 0)
            error("ERROR writing to socket");

        close(childfd);

        if (to_us(get_current_time() - start) > 1000000) {
//            f.close();
//            f = std::ofstream{"../result.txt"};
//            f << connectcnt << std::endl;
            start = get_current_time();
//            v_conn_per_sec.push_back(alive_connections);
            std::cout << "[Loop " << loop << "]" "Alive connections from " << std::this_thread::get_id() << ":" << cnt << std::endl;
            cnt = 0;
            loop++;
        }
    }
    close(parentfd);
}

int main(int argc, char **argv) {
//    int parentfd; /* parent socket */
//    int portno; /* port to listen on */
//    socklen_t clientlen; /* byte size of client's address */
//    struct sockaddr_in serveraddr; /* server's addr */
//    struct sockaddr_in clientaddr; /* client addr */
//    char buf[BUFSIZE]; /* message buffer */
//    int optval; /* flag value for setsockopt */
//    int n; /* message byte size */
    int notdone;
//    fd_set readfds;
//
//    /*
//     * check command line arguments
//     */
//    if (argc != 2) {
//        fprintf(stderr, "usage: %s <port>\n", argv[0]);
//        exit(1);
//    }
//    portno = atoi(argv[1]);
//
//    /*
//     * socket: create the parent socket
//     */
//    parentfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (parentfd < 0)
//        error("ERROR opening socket");
//
//    /* setsockopt: Handy debugging trick that lets
//     * us rerun the server immediately after we kill it;
//     * otherwise we have to wait about 20 secs.
//     * Eliminates "ERROR on binding: Address already in use" error.
//     */
//    optval = 1;
//    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
//               (const void *)&optval , sizeof(int));
//
//    /*
//     * build the server's Internet address
//     */
//    bzero((char *) &serveraddr, sizeof(serveraddr));
//
//    /* this is an Internet address */
//    serveraddr.sin_family = AF_INET;
//
//    /* let the system figure out our IP address */
//    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//    /* this is the port we will listen on */
//    serveraddr.sin_port = htons((unsigned short)portno);
//
//    /*
//     * bind: associate the parent socket with a port
//     */
//    if (bind(parentfd, (struct sockaddr *) &serveraddr,
//             sizeof(serveraddr)) < 0)
//        error("ERROR on binding");
//
//    /*
//     * listen: make this socket ready to accept connection requests
//     */
//    if (listen(parentfd, 100) < 0) /* allow 5 requests to queue up */
//        error("ERROR on listen");
//
//
//    /* initialize some things for the main loop */
//    clientlen = sizeof(clientaddr);
    notdone = 1;
//
//    printf("server> ");
//    fflush(stdout);
//
//    /*
//     * main loop: wait for connection request or stdin command.
//     *
//     * If connection request, then echo input line
//     * and close connection.
//     * If command, then process command.
//     */
    std::vector<std::thread> v;
    if (notdone) {

        /*
         * select: Has the user typed something to stdin or
         * has a connection request arrived?
         */
//        FD_ZERO(&readfds);          /* initialize the fd set */
//        FD_SET(parentfd, &readfds); /* add socket fd */
//        if (select(parentfd+1, &readfds, 0, 0, 0) < 0) {
//            error("ERROR in select");
//        }
        for (int i = 0; i < 4; i++) {
            v.push_back(std::thread (&acceptoring));
        }


        /* if a connection request has arrived, process it */
    }

    /* clean up */
    for (auto &t: v) {
        t.join();
    }
    printf("Terminating server.\n");
    exit(0);
}