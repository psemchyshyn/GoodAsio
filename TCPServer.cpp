#include "TCPServer.h"
#include <boost/bind.hpp>

TCPServer::TCPServer(boost::asio::io_context& io_context):
    io_context_(io_context),
    acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1024))
    {
        start_accept();
    }


void TCPServer::start_accept()  {
    std::shared_ptr<TCPConnection> new_connection = TCPConnection::create(io_context_, *this);
    for (int i = 0; i < 10; i++) {
        acceptor_.async_accept(new_connection->get_socket(),
                               boost::bind(&TCPServer::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error));
    }
}

void TCPServer::handle_accept(std::shared_ptr<TCPConnection> new_connection, const boost::system::error_code &error) {
    {
        if (!error) {
            connections_alive++;
            new_connection->start();
        }
        start_accept();
    }
}

