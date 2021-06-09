//
// Created by msemc on 12.05.2021.
//

#include "TCPServer.h"
#include <iostream>
#include <thread>
#include <vector>
#include <boost/bind/bind.hpp>

void runEventLoop(boost::asio::io_context *e) {
    e->run();
}

int main()
{
    std::vector<std::thread> threads;
    try {
        for (int i = 0; i < 3; i++) {
            auto io_context = new boost::asio::io_context{};
            auto server = new TCPServer(*io_context);
            threads.push_back(std::thread(&runEventLoop, io_context));
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    for (auto &t: threads) {
        t.join();
    }
    return 0;
}