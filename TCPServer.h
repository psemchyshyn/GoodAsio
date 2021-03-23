#ifndef ECHO_SERVER_TCPSERVER_H
#define ECHO_SERVER_TCPSERVER_H
#include <boost/asio.hpp>
#include "TCPConnection.h"

using boost::asio::ip::tcp;

class TCPServer {
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    int connections_alive;

    friend class TCPConnection;

    void start_accept();
    void handle_accept(std::shared_ptr<TCPConnection> new_connection, const boost::system::error_code &error);

public:
    explicit TCPServer(boost::asio::io_context& io_context);
};
#endif //ECHO_SERVER_TCPSERVER_H
