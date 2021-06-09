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

    std::vector<struct pollfd> tracking;
    struct pollfd parent;
    parent.fd = parentfd;
    parent.events = POLLIN;
    tracking.push_back(parent);
    int num_ready;

    auto start = get_current_time();

    while (true) {
        if (poll(tracking.data(), tracking.size(), 0) == -1) {
            throw "Error while polling";
        }
        std::set<int> to_remove;
        std::vector<struct pollfd> extend;
        for (int i = 0; i < tracking.size(); i++) {
            if (tracking.at(i).revents & POLLIN) {
                if (tracking.at(i).fd == parentfd) {
                    childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
//                    std::cout << "Accepted: " << childfd << '\n';
                    struct pollfd curr;
                    curr.fd = childfd;
                    curr.events = POLLIN;
                    connectcnt++;
                    extend.push_back(curr);
                } else {
                    bzero(buf, BUFSIZE);
                    n = read(tracking.at(i).fd, buf, BUFSIZE);
                    if (n < 0)
                        throw "ERROR reading from socket sdf";
                    tracking.at(i).events = POLLOUT;
                }
            } else if (tracking.at(i).revents & POLLOUT) {
                n = write(tracking.at(i).fd, buf, strlen(buf));
                if (n < 0)
                    throw "ERROR writing to socket";
                to_remove.insert(tracking.at(i).fd);
                close(tracking.at(i).fd);
            }
        }
        tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&to_remove](struct pollfd e) {
            return to_remove.find(e.fd) != to_remove.end();
        }), tracking.end());
        tracking.insert(tracking.end(), extend.begin(), extend.end());

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

