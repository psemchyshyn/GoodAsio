//
// Created by msemc on 22.03.2021.
//

#include "TCPConnection.h"
#include "TCPServer.h"

#include "boost/bind.hpp"
#include <iostream>



TCPConnection::TCPConnection(boost::asio::io_context& io_context, TCPServer& server_):
    socket_(io_context),
    server{server_}
    {
    }


void TCPConnection::start() {
//        auto start = get_current_time();
    socket_.wait(socket_.wait_read);
    int bytes = socket_.available();
    buf.resize(bytes);
//        std::cout << bytes << "\n";
    boost::asio::async_read(socket_, boost::asio::buffer(buf.data(), buf.size()), boost::bind(
            &TCPConnection::handle_read, this
    ));

//        auto total = get_current_time() - start;
//        std::cout << "Time of async write is: " << to_us(total) << "\n";
}

void TCPConnection::handle_write(const boost::system::error_code&, int bytes_sent) {
    std::cout << "Sent!\n";
}

void TCPConnection::handle_read() {
    for (auto ch : buf) {
        std::cout << ch;
    }
    boost::asio::async_write(socket_, boost::asio::buffer(buf.data(), buf.size()),
                             boost::bind(&TCPConnection::handle_write, this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

tcp::socket& TCPConnection::get_socket() {
    return socket_;
}

void TCPConnection::disconnect() {
    server.connections_alive--;
}
