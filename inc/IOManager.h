//
// Created by msemc on 12.05.2021.
//
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
#include <poll.h>
#include <cmath>
#include <unordered_map>


#ifndef ECHO_SERVER_IOMANAGER_H
#define ECHO_SERVER_IOMANAGER_H
class IOManager {
public:
    virtual void register_read_fd(int fd) =0;
    virtual void register_write_fd(int fd) =0;
    virtual bool is_set_read(int fd) =0;
    virtual bool is_set_write(int fd) =0;
    virtual void update_all(int timeout) =0;
};

class SelectError: public std::runtime_error {
public:
    explicit SelectError(const std::string& message): runtime_error(message){};
};

class PollError: public std::runtime_error {
public:
    explicit PollError(const std::string& message): runtime_error(message){};
};

class Selector: public IOManager{
    fd_set master_read_fds;
    fd_set master_write_fds;
    fd_set reads_alive_set;
    fd_set writes_alive_set;
    int max_descriptor = 0;

public:
    Selector() =default;

    void register_read_fd(int fd) override {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_read_fds);
    }

    void register_write_fd(int fd) override {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        FD_SET(fd, &master_write_fds);
    }

    void update_all(int timeout) override {
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


class Poller: public IOManager {
    std::unordered_map<int, struct pollfd> tracker;
//    std::vector<struct pollfd> tracking;
public:
    void register_read_fd(int fd) {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLIN;
//        tracking.push_back(curr);
        tracker[fd] = curr;
    }

    void register_write_fd(int fd) {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLOUT;
//        tracking.push_back(curr);
        tracker[fd] = curr;
    }

    void update_all(int timeout) {
        std::vector<struct pollfd> temp(tracker.size());
        transform(tracker.begin(), tracker.end(), temp.begin(), [](auto pair){return pair.second;});
        if (poll(temp.data(), temp.size(), timeout) == -1) {
            throw "Error while polling";
        }
    }



    bool is_set_read(int fd) {
        auto curr = tracker.find(fd);
        if (curr != tracker.end() && (curr->second.revents & POLLIN)) {
            tracker.erase(curr);
            return true;
        }
        return false;
    }


    bool is_set_write(int fd) {
        auto curr = tracker.find(fd);
        if (curr != tracker.end() && (curr->second.revents & POLLOUT)) {
            tracker.erase(curr);
            return true;
        }
        return false;
    }
};

class Pollerr {
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

    std::set<std::pair<int, char>> extract_all(int timeout) {
        if (poll(tracking.data(), tracking.size(), timeout) == -1) {
            throw "Error while polling";
        }
        std::set<std::pair<int, char>> result;
        for (size_t i = 0; i < tracking.size(); i++) {
            if (tracking.at(i).revents & POLLIN) {
                result.insert(std::pair{tracking.at(i).fd, 'r'});
            } else if (tracking.at(i).revents & POLLOUT) {
                result.insert(std::pair{tracking.at(i).fd, 'w'});
            }
        }
        tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&result](struct pollfd e) {
            return result.find(std::pair{e.fd, 'r'}) != result.end() || result.find(std::pair{e.fd, 'w'}) != result.end();
        }), tracking.end());
        return result;
    }
};

#endif //ECHO_SERVER_IOMANAGER_H
