//#include "TCPServer.h"
//#include <iostream>
//#include <boost/bind/bind.hpp>
#include <functional>
#include <iostream>
#include "Event.hpp"



// TOD: bug with session_pull, add read/write to sockets
#include "EventLoop.h"
#include "Sockets.h"

void hello(){
    std::cout << "Contagious" << "\n";
}

int main()
{
    EventLoop e{};
    auto server = ServerSocket::createServer(1024);
    server->async_accept(e, std::bind(&hello));

//    try {
//        boost::asio::io_context io_context;
//        TCPServer server(io_context);
//        io_context.run();
//        std::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
//        t.join();
//    } catch (std::exception& e) {
//        std::cerr << e.what() << std::endl;
//    }
    return 0;
}
