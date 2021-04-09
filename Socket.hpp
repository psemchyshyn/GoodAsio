//
// Created by msemc on 09.04.2021.
//

#ifndef ECHO_SERVER_SOCKET_HPP
#define ECHO_SERVER_SOCKET_HPP
#include <string>
#include "EventLoop.h"
#include "Event.hpp"

class ServerSocket;

class Socket {
protected:
    int fd_;
public:
    explicit Socket(int fd): fd_(fd){};
    int get_fd() {
        return fd_;
    }
};

class SessionSocket: public Socket {

    friend ServerSocket;
    std::string buf_{};
    explicit SessionSocket(int fd, std::string buf = ""): Socket(fd), buf_{buf} {};

public:
    template<typename Callback> void async_read(EventLoop e_loop, Callback callback) {
        e_loop.addEvent(new ReadEvent(this, callback));
    }

    template<typename Callback> void async_write(EventLoop e_loop, Callback callback) {
        e_loop.addEvent(new WriteEvent(this, callback));
    }
};

//
//class SocketFactory {
//    static createServer(int port) {
//
//    }
//
//    Socket* createSession(int fd) {
//        return new SessionSocket(fd);
//    }
//};

class ServerSocket: public Socket {
//    friend SocketFactory

    ServerSocket(int fd);
public:
    template<typename Callback> void async_accept(EventLoop e_loop, Callback callback) {

    }
};

#endif //ECHO_SERVER_SOCKET_HPP
