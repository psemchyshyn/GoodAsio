#include "TCPServer.h"
#include <boost/bind/bind.hpp>
#include "../inc/my_time.h"

TCPServer::TCPServer(boost::asio::io_context& io_context):
    io_context_(io_context),
    acceptor_(io_context)
    {
        acceptor_.open(tcp::v4());
        int one = 1;
        setsockopt(acceptor_.native_handle(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &one, sizeof(one));
//        acceptor_.set_option(boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>(true));
        acceptor_.bind(tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1024));
        acceptor_.listen();
        start = get_current_time();
        start_accept();
    }


void TCPServer::start_accept()  {
    std::shared_ptr<TCPConnection> new_connection = TCPConnection::create(io_context_, *this);

        acceptor_.async_accept(new_connection->get_socket(),
                               boost::bind(&TCPServer::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error));
}

void TCPServer::handle_accept(const std::shared_ptr<TCPConnection>& new_connection, const boost::system::error_code &error) {
    {
        if (!error) {
            new_connection->start();
        }
        start_accept();
    }
}
