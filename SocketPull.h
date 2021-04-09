//
// Created by msemc on 09.04.2021.
//

#ifndef ECHO_SERVER_SOCKETPULL_H
#define ECHO_SERVER_SOCKETPULL_H
#include "Selector.h"
#include "Socket.hpp"

class SocketPull {
    Selector selector{};
    std::set<Socket*> sockets;
public:
    SocketPull() =default;
    void register_read(Socket* socket);
    void register_write(Socket* socket);
    std::set<Socket*> getFreeSockets(int timeout);
};

#endif //ECHO_SERVER_SOCKETPULL_H
