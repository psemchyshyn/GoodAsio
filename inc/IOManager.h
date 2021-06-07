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
    virtual std::set<std::pair<int, char>> extract_all(int timeout) =0;
};

class SelectError: public std::runtime_error {
public:
    explicit SelectError(const std::string& message): runtime_error(message){};
};

class PollError: public std::runtime_error {
public:
    explicit PollError(const std::string& message): runtime_error(message){};
};

class EPollError: public std::runtime_error {
public:
    explicit EPollError(const std::string& message): runtime_error(message){};
};

class Selector: public IOManager{
    fd_set master_read_fds;
    fd_set master_write_fds;
    std::vector<std::pair<int, char>> tracking;
    int max_descriptor = 0;

public:
    Selector() =default;

    void register_read_fd(int fd) override {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        tracking.push_back(std::pair{fd, 'r'});
        FD_SET(fd, &master_read_fds);
    }

    void register_write_fd(int fd) override {
        if (fd > max_descriptor) {
            max_descriptor = fd;
        }
        tracking.push_back(std::pair{fd, 'w'});
        FD_SET(fd, &master_write_fds);
    }

    std::set<std::pair<int, char>> extract_all(int timeout) override {
        fd_set reads_alive_set = master_read_fds;
        fd_set writes_alive_set = master_write_fds; // later reads_set will be cleared of those descriptors that can't read
        struct timeval t{0, timeout};
        if (select(max_descriptor + 1, &reads_alive_set, &writes_alive_set, 0, &t) < 0) {
            throw SelectError{"Error in select"};
        }

        std::set<std::pair<int, char>> result;
        for (size_t i = 0; i < tracking.size(); i++) {
            if (tracking.at(i).second == 'r' && FD_ISSET(tracking.at(i).first, &reads_alive_set)) {
                result.insert(tracking.at(i));
                FD_CLR(tracking.at(i).first, &master_read_fds);
            } else if (tracking.at(i).second == 'w' && FD_ISSET(tracking.at(i).first, &writes_alive_set)) {
                result.insert(tracking.at(i));
                FD_CLR(tracking.at(i).first, &master_write_fds);
            }
        }

        tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&result](std::pair<int, char> e) {
            return result.find(e) != result.end();
        }), tracking.end());
        return result;
    }
};


class Poller: public IOManager {
    std::vector<struct pollfd> tracking;
public:
    void register_read_fd(int fd) override {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLIN;
        tracking.push_back(curr);
    }

    void register_write_fd(int fd) override {
        struct pollfd curr;
        curr.fd = fd;
        curr.events = POLLOUT;
        tracking.push_back(curr);
    }

    std::set<std::pair<int, char>> extract_all(int timeout) override {
        if (poll(tracking.data(), tracking.size(), timeout) == -1) {
            throw PollError{"error in poll"};
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

class Epoller: public IOManager {
    static const int EVENTS_SIZE = 10000;
    struct epoll_event events[EVENTS_SIZE];
    int epoll_instance_fd;
public:
    Epoller(): IOManager() {
        epoll_instance_fd = epoll_create1(0);
    }

    void register_read_fd(int fd) override {
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN;
        epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, fd, &ev);
    }

    void register_write_fd(int fd) override {
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLOUT;
        epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, fd, &ev);
    }

    std::set<std::pair<int, char>> extract_all(int timeout) override {
        auto ready_num = epoll_wait(epoll_instance_fd, events, EVENTS_SIZE, timeout);
        if (ready_num == -1) {
            throw "Error while polling";
        }
        std::set<std::pair<int, char>> result;
        for (size_t i = 0; i < ready_num; i++) {
            int desc = events[i].data.fd;
            if (events[i].events == POLLIN) {
                result.insert(std::pair{desc, 'r'});
            } else if (events[i].events == POLLOUT) {
                result.insert(std::pair{desc, 'w'});
            }
            epoll_ctl(epoll_instance_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
        }
        return result;
    }
};

#endif //ECHO_SERVER_IOMANAGER_H
