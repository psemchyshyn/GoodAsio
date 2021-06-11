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

    void register_read_fd(int fd) override;

    void register_write_fd(int fd) override;

    std::set<std::pair<int, char>> extract_all(int timeout) override;
};


class Poller: public IOManager {
    std::vector<struct pollfd> tracking;
public:
    void register_read_fd(int fd) override;

    void register_write_fd(int fd) override;

    std::set<std::pair<int, char>> extract_all(int timeout) override;
};

class Epoller: public IOManager {
    static const int EVENTS_SIZE = 100000;
    struct epoll_event events[EVENTS_SIZE];
    int epoll_instance_fd;
public:
    Epoller();

    void register_read_fd(int fd) override;

    void register_write_fd(int fd) override;

    std::set<std::pair<int, char>> extract_all(int timeout) override;
};

#endif //ECHO_SERVER_IOMANAGER_H
