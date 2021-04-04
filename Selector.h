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

class Selector {
    fd_set read_fds;
    fd_set write_fds;
    std::unordered_map<int, std::function<void(int, void*, size_t)>> read_v_fds;
    std::unordered_map<int, std::function<void(int, void*, size_t)>> write_v_fds;

public:
    Selector() {

    }

    void register_fd(int fd, bool set_read, bool set_write) {
        if (set_read) {
            FD_SET(fd, &read_fds);
            read_v_fds.push_back(fd);
        }
        if (set_write) {
            FD_SET(fd, &write_fds);
            write_v_fds.push_back(fd);
        }
    }

    std::vector<int> get_available(int timeout) {
        std::vector<int> res;
        int max_read_fd = *std::max_element(read_v_fds.begin(), read_v_fds.end());
        int max_write_fd = *std::max_element(write_v_fds.begin(), write_v_fds.end());
        int max_fd = max_read_fd < max_write_fd ? max_write_fd : max_read_fd;
        struct timeval t{timeout, timeout*1000000}; // Розібратися чи тут сума чи однакові значення
        select(max_fd, &read_fds, &write_fds, 0, &t);
        for (auto fd : read_v_fds) {
            if (FD_ISSET(fd, &read_fds)) {
                res.push_back(fd);
//                read_v_fds.pop_back(); ремувати фд з мапи
            }
        }
        for (auto fd: write_v_fds) {
            if (FD_ISSET(fd, &write_fds)) {
                res.push_back(fd);
//                write_v_fds.pop_back() теж ремувати
            }
        }

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        for (auto fd: read_v_fds) {
            FD_SET(fd, &read_fds);
        }
        for (auto fd: write_v_fds) {
            FD_SET(fd, &write_fds);
        }

        return res;

    }
};


#endif //ECHO_SERVER_SELECTOR_H
