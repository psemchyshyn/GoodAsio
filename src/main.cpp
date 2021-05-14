//#include "TCPServer.h"
//#include <iostream>
//#include <boost/bind/bind.hpp>
#include <functional>
#include <iostream>

#include "../inc/EventLoop.h"
#include "../inc/Sockets.h"
// TOD: bug with session_pull, add read/write to sockets

class EchoServer {
    EventLoop& context;
    ServerSocket* acceptor;
    std::chrono::high_resolution_clock::time_point start;
    size_t connections_alive = 0;
public:
    EchoServer(EventLoop& context_, ServerSocket* acceptor_):
        context{context_},
        acceptor{acceptor_}
    {
        start = get_current_time();
        start_accept();
    }

    ~EchoServer(){
        delete acceptor;
    }

    void start_accept() {
        acceptor->async_accept(context, std::bind(&EchoServer::handle_accept, this));
    }

    void handle_write(SessionSocket* socket) {
//        std::cout << "write finished\n";
        delete socket;
    }

    void handle_read(SessionSocket* s) {
//        std::cout << s->get_buf() << '\n';
//        std::cout << "init write\n";
        s->async_write(context, std::bind(&EchoServer::handle_write, this, s));
    }

    void handle_accept(){
//        std::cout << "In accept\n";
        connections_alive++;
        auto session = acceptor->getCurrentSession();
        session->async_read(context, std::bind(&EchoServer::handle_read, this, session));
        start_accept();
        if (to_us(get_current_time() - start) > 1000000) {
            start = get_current_time();
//            v_conn_per_sec.push_back(server.connections_alive);
            std::cout << "Connections alive: " << connections_alive << '\n';
            connections_alive = 0;
        }
    }

};



int main()
{
    EventLoop e{};
    FactoryServer factory{};
    auto server = factory.createServer(1024);
    EchoServer s{e, server};
    e.run();
    return 0;
}
