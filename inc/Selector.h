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
#include "my_time.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <cmath>

class SelectError: public std::runtime_error {
public:
    explicit SelectError(const std::string& message): runtime_error(message){};
};

class Selector {
    fd_set master_read_fds;
    fd_set master_write_fds;
    std::set<int> alive_reads;
    std::set<int> alive_writes;
    int max_descriptor = 0;

public:
    Selector() {

    }

    void register_read_fd(int fd) {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_read_fds);
        alive_reads.insert(fd);
    }

    void register_write_fd(int fd) {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_write_fds);
        alive_writes.insert(fd);
    }

    std::set<int> extract_readables(int timeout) { // in micro seconds
        std::set<int> result;
        fd_set reads_set = master_read_fds; // later reads_set will be cleared of those descriptors that can't read
        struct timeval t{0, timeout};
        if (select(max_descriptor + 1, &reads_set, 0, 0, &t) < 0) {
            throw SelectError{"Error in select"};
        }
        auto temp = alive_reads;
        for (auto fd: temp) {
            if (FD_ISSET(fd, &reads_set)) {
                result.insert(fd);
                alive_reads.extract(fd);
                FD_CLR(fd, &master_read_fds);
            }
        }
        return result;
    }

    std::set<int> extract_writeables(int timeout) { // in micro seconds
        std::set<int> result;
        fd_set write_set = master_write_fds; // later reads_set will be cleared of those descriptors that can't read
        struct timeval t{0, timeout};
        if (select(max_descriptor + 1, 0, &write_set, 0, &t) < 0) {
            throw SelectError{"Error in select"};
        }
        auto temp = alive_writes;
        for (auto fd: temp) {
            if (FD_ISSET(fd, &write_set)) {
                result.insert(fd);
                alive_writes.extract(fd);
                FD_CLR(fd, &master_write_fds);
            }
        }
        return result;
    }

    int get_max_descriptor() {
        return max_descriptor;
    }
};


#endif //ECHO_SERVER_SELECTOR_H
