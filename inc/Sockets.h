#ifndef ECHO_SERVER_SOCKETS_H
#define ECHO_SERVER_SOCKETS_H

#include "SocketTemplate.h"

class SessionSocket: public Socket {

    friend ServerSocket;
    std::string buf_;
    explicit SessionSocket(int fd, size_t buf_size = 20): Socket(fd), buf_(buf_size, ' ') {};

public:
    template<typename Callback> void async_read(EventLoop& e_loop, Callback callback) {
        e_loop.addEvent(new ReadEvent(this, callback));
    }

    template<typename Callback> void async_write(EventLoop& e_loop, Callback callback) {
        e_loop.addEvent(new WriteEvent(this, callback));
    }

    void sread() override {
        bzero(buf_.data(), buf_.size());
        ssize_t n = read(fd_, buf_.data(), buf_.size());
//        std::cout << "start read: " << fd_ << "\n";

        if (n < 0)
            throw "Something went wrong with read"; // read exception later
    }

    void swrite() override {
        ssize_t n = write(fd_, buf_.data(), buf_.size());

//        std::cout << "start write: " << fd_ << "\n";

        if (n < 0)
            throw "Something went wrong with write";
    }

    std::string get_buf() {
        return buf_;
    }
};


class FactoryServer;

class ServerSocket: public Socket {
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr); /* byte size of client's address */
    SessionSocket* current_session;

    friend FactoryServer;

    explicit ServerSocket(int fd): Socket(fd){};
public:
    template<typename Callback> void async_accept(EventLoop& e_loop, Callback callback) {
        e_loop.addEvent(new ReadEvent<Callback>{this, callback});
    }
    void sread() {
        int childfd = accept(fd_, (struct sockaddr *) &clientaddr, &clientlen);
//        std::cout << "Accepted: " << childfd << "\n";
        current_session = new SessionSocket(childfd);
    }

    void swrite() {
        // Pavlo:I don't know what to put here
        // Danylo: Do we even need this for now? Is it like server reading from other server?
    }

    SessionSocket* getCurrentSession() {
        return current_session;
    }
};


class FactoryServer {
public:
    explicit FactoryServer() =default;
    ServerSocket* createServer(int portno) {
        /*
         * socket: create the parent socket
         */
        struct sockaddr_in serveraddr;
        int parentfd = socket(AF_INET, SOCK_STREAM, 0);
        if (parentfd < 0)
            throw "ERROR opening socket";

        /* setsockopt: Handy debugging trick that lets
         * us rerun the server immediately after we kill it;
         * otherwise we have to wait about 20 secs.
         * Eliminates "ERROR on binding: Address already in use" error.
         */
        int optval = 1;
        setsockopt(parentfd, SOL_SOCKET, SO_REUSEPORT,
                   (const void *) &optval, sizeof(int));

        /*
         * build the server's Internet address
         */
        bzero((char *) &serveraddr, sizeof(serveraddr));

        /* this is an Internet address */
        serveraddr.sin_family = AF_INET;

        /* let the system figure out our IP address */
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

        /* this is the port we will listen on */
        serveraddr.sin_port = htons((unsigned short) portno);

        /*
         * bind: associate the parent socket with a port
         */
        if (bind(parentfd, (struct sockaddr *) &serveraddr,
                 sizeof(serveraddr)) < 0)
            throw "ERROR on binding";

        /*
         * listen: make this socket ready to accept connection requests
         */
        if (listen(parentfd, 10000) < 0) /* allow 10000 requests to queue up */
            throw "ERROR on listen";
        return new ServerSocket(parentfd);
    }
};

#endif //ECHO_SERVER_SOCKETS_H
