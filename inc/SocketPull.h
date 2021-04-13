#ifndef ECHO_SERVER_SOCKETPULL_H
#define ECHO_SERVER_SOCKETPULL_H
#include "Selector.h"
#include "SocketTemplate.h"

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
