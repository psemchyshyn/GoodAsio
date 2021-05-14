//
// Created by msemc on 05.05.2021.
//

#ifndef ECHO_SERVER_POLLER_H
#define ECHO_SERVER_POLLER_H

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
#include "../inc/my_time.h"
#include <vector>
#include <algorithm>
#include <set>

int BUFSIZE = 10;

class Poller {
    std::vector<struct pollfd> tracking;

public:
    void register_read_fd(int fd) {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLIN;
        tracking.push_back(curr);
    }

    void register_write_fd(int fd) {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLOUT;
        tracking.push_back(curr);
    }

    std::set<int> extract_all(int timeout) {
        if (poll(tracking.data(), tracking.size(), timeout) == -1) {
            throw "Error while polling";
        }
        std::set<int> result;
        for (size_t i = 0; i < tracking.size(); i++) {
            if (tracking.at(i).events & POLLIN || tracking.at(i).events & POLLOUT) {
                result.insert(tracking.at(i).fd);
            }
        }
        tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&result](struct pollfd e) {
            return result.find(e.fd) != result.end();
        }), tracking.end());
        return result;
    }
}



#endif //ECHO_SERVER_POLLER_H
