#ifndef ECHO_SERVER_TCPSERVER_H
#define ECHO_SERVER_TCPSERVER_H
#include <boost/asio.hpp>
#include <atomic>
#include "TCPConnection.h"
#include "my_time.h"

using boost::asio::ip::tcp;

class TCPServer {
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    int connections_alive = 0;
    std::vector<int> v_conn_per_sec{};
    std::string response{"Client's request"};
    std::chrono::high_resolution_clock::time_point start;
    int limit = 10000;
    int current_accepts = 0;

    friend class TCPConnection;

    void start_accept();
    void handle_accept(const std::shared_ptr<TCPConnection>& new_connection, const boost::system::error_code &error);
public:
    explicit TCPServer(boost::asio::io_context& io_context);
};
#endif //ECHO_SERVER_TCPSERVER_H
