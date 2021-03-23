//
// Created by msemc on 23.03.2021.
//

#include "TCPServer.h"
#include <iostream>

int main()
{
    try {
        boost::asio::io_context io_context;
        TCPServer server(io_context);
        io_context.run();
//        std::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
//        t.join();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
