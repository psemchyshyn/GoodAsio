#ifndef ECHO_SERVER_SELECTOR_H
#define ECHO_SERVER_SELECTOR_H

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
#include "TimeMeasurement.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <cmath>

class SelectError: public std::runtime_error {
public:
    explicit SelectError(const std::string& message): runtime_error(message){};
};

class Selector{
    fd_set master_read_fds;
    fd_set master_write_fds;
    fd_set reads_alive_set;
    fd_set writes_alive_set;
    int max_descriptor = 0;

public:
    Selector() =default;

    void register_read_fd(int fd) {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_read_fds);
    }

    void register_write_fd(int fd) {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_write_fds);
    }

    void update_all(int timeout) {
        reads_alive_set = master_read_fds;
        writes_alive_set = master_write_fds; // later reads_set will be cleared of those descriptors that can't read
        struct timeval t{0, timeout};
        if (select(max_descriptor + 1, &reads_alive_set, &writes_alive_set, 0, &t) < 0) {
            throw SelectError{"Error in select"};
        }
    }

    bool is_set_read(int fd) {
        bool is_set = FD_ISSET(fd, &reads_alive_set);
        if (is_set) {
            FD_CLR(fd, &master_read_fds);
        }
        return is_set;
    }

    bool is_set_write(int fd) {
        bool is_set = FD_ISSET(fd, &writes_alive_set);
        if (is_set) {
            FD_CLR(fd, &master_write_fds);
        }
        return is_set;
    }

    int get_max_descriptor() {
        return max_descriptor;
    }
};


#endif //ECHO_SERVER_SELECTOR_H
