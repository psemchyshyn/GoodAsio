//
// Created by msemc on 04.05.2021.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include "../inc/TimeMeasurement.h"
#include <vector>
#include <algorithm>
#include <set>

int BUFSIZE = 10;
int max_events = 100000;

int main(int argc, char** argv) {

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

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0)
        throw "Error opening socket";
    optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *) &optval, sizeof(int));
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) portno);

    if (bind(parentfd, (struct sockaddr *) &serveraddr,
             sizeof(serveraddr)) < 0)
        throw "ERROR on binding";

    if (listen(parentfd, 10000) < 0) /* allow 5 requests to queue up */
        throw "ERROR on listen";

    clientlen = sizeof(clientaddr);
    connectcnt = 0;


    int epoll_fd = epoll_create1(0);
    struct epoll_event ev, events[max_events];
    ev.data.fd = parentfd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, parentfd, &ev);

    std::vector<struct epoll_event> tracking;
    auto start = get_current_time();
    while (true) {
        auto ready_num = epoll_wait(epoll_fd, events, max_events, 0);
        if (ready_num == -1) {
            throw "Error while polling";
        }
        for (int i = 0; i < ready_num; i++) {
            if (events[i].events == EPOLLIN) {
                if (events[i].data.fd  == parentfd) {
                    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
                    struct epoll_event ev;
                    ev.data.fd = childfd;
                    ev.events = EPOLLIN;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, childfd, &ev);
                    connectcnt++;
                } else {
                    bzero(buf, BUFSIZE);
                    n = read(events[i].data.fd, buf, BUFSIZE);
                    if (n < 0)
                        throw "ERROR reading from socket sdf";
                    struct epoll_event ev;
                    ev.events = EPOLLOUT;
                    ev.data.fd = events[i].data.fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
                }
            } else if (events[i].events == EPOLLOUT) {
                n = write(events[i].data.fd, buf, strlen(buf));
                if (n < 0)
                    throw "ERROR writing to socket";
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
                close(events[i].data.fd);
            }
        }
        if (to_us(get_current_time() - start) > 1000000) {
//            f.close();
//            f = std::ofstream{"../result.txt"};
//            f << connectcnt << std::endl;
            start = get_current_time();
//            v_conn_per_sec.push_back(alive_connections);
            std::cout << "Alive connections: " << connectcnt << std::endl;
            connectcnt = 0;
        }
    }
    return 0;
}

