//#include "TCPServer.h"
//#include <iostream>
//#include <boost/bind/bind.hpp>
#include <functional>
#include <iostream>
#include <thread>

#include "../inc/my_time.h"
#include "../inc/EventLoop.h"
#include "../inc/Sockets.h"

class EchoServer {
    EventLoop& context;
    ServerSocket* acceptor;
    std::chrono::high_resolution_clock::time_point start;
    size_t connections_alive = 0;
    int loop = 0;
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
            std::cout << "[Loop " << loop << "]" "Alive connections from " << std::this_thread::get_id() << ":" << connections_alive << std::endl;
            connections_alive = 0;
            loop++;
        }
    }

    int get_connections_alive() {
        auto result = connections_alive;
//        connections_alive = 0;
        return result;
    }

};


void runEventLoop(EventLoop *e) {
    e->run();
}

int main()
{
    auto start = get_current_time();
    FactoryServer factory{};
    std::vector<EchoServer> servers;
    std::vector<EventLoop*> event_loops;
    std::vector<std::thread> threads;

    for (int i = 0; i < 2; i++) {

        EventLoop *e = new EventLoop();
        auto server = factory.createServer(1024);
        EchoServer *t = new EchoServer{*e, server};
        threads.push_back(std::thread(&runEventLoop, e));
        event_loops.push_back(e);
    }

//    while (true) {
//        if (to_us(get_current_time() - start) > 1000000) {
////            f.close();
////            f = std::ofstream{"../result.txt"};
////            f << connectcnt << std::endl;
//            start = get_current_time();
////            v_conn_per_sec.push_back(alive_connections);
//            int total_cnt = 0;
//            std::cout << "Start" << '\n';
//            for (auto &server: servers) {
//                auto cnt = server.get_connections_alive();
//                std::cout << cnt << '\n';
//                total_cnt += cnt;
//            }
//            std::cout << "finish" << '\n';
//            std::cout << "Alive connections: " << total_cnt << std::endl;
//        }
//    }

//
    for (auto &t: threads) {
        t.join();
    }

    return 0;
}
