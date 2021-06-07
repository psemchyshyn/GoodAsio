#include "TCPConnection.h"
#include "TCPServer.h"
#include "../inc/my_time.h"
#include "boost/bind/bind.hpp"
#include <iostream>



TCPConnection::TCPConnection(boost::asio::io_context& io_context, TCPServer& server_):
    socket_(io_context),
    server{server_}
    {
    }


void TCPConnection::start() {
//        auto start = get_current_time();
    int bytes = socket_.available();
    buf.resize(bytes);
//        std::cout << bytes << "\n";
    boost::asio::async_read(socket_, boost::asio::buffer(buf.data(), buf.size()), boost::bind(
            &TCPConnection::handle_read, shared_from_this()
    ));

//        auto total = get_current_time() - start;
//        std::cout << "Time of async write is: " << to_us(total) << "\n";
}

void TCPConnection::handle_write(const boost::system::error_code&, int bytes_sent) {

    server.connections_alive++;
    count();
}

void TCPConnection::handle_read() {
    boost::asio::async_write(socket_, boost::asio::buffer(buf.data(), buf.size()),
                             boost::bind(&TCPConnection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

tcp::socket& TCPConnection::get_socket() {
    return socket_;
}

void TCPConnection::disconnect() {
    server.connections_alive--;
}

void TCPConnection::count() {
    if (to_us(get_current_time() - server.start) > 1000000) {
        server.start = get_current_time();
        server.v_conn_per_sec.push_back(server.connections_alive);
        std::cout << "[Loop " << server.loop << "]" "Alive connections from " << ":" << server.connections_alive << std::endl;
        server.connections_alive = 0;
        server.loop++;
    }
};