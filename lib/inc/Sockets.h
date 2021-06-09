#ifndef ECHO_SERVER_SOCKETS_H
#define ECHO_SERVER_SOCKETS_H

#include "SocketTemplate.h"
#include "EventLoop.h"

class SessionSocket: public Socket {
    friend ServerSocket;
    std::string buf_;
    explicit SessionSocket(int fd, size_t buf_size = 20): Socket(fd), buf_(buf_size, ' ') {};
public:
    template<typename Callback>
    void async_read(EventLoop &e_loop, Callback callback) {
        e_loop.addEvent(new ReadEvent(this, callback));
    }

    template<typename Callback>
    void async_write(EventLoop &e_loop, Callback callback) {
        e_loop.addEvent(new WriteEvent(this, callback));
    }

    void sread() override;

    void swrite() override;

    std::string get_buf();
};

class FactoryServer;

class ServerSocket: public Socket {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr); /* byte size of client's address */
    SessionSocket* current_session;

    friend FactoryServer;

    explicit ServerSocket(int fd): Socket(fd){};
public:
    template<typename Callback>
    void async_accept(EventLoop &e_loop, Callback callback) {
        e_loop.addEvent(new ReadEvent<Callback>{this, callback});
    }

    void sread();

    SessionSocket* getCurrentSession();
};

class FactoryServer {
public:
    explicit FactoryServer() =default;

    ServerSocket* createServer(int portno);
};

#endif //ECHO_SERVER_SOCKETS_H
