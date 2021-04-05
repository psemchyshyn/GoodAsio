#ifndef ECHO_SERVER_TCPCONNECTION_H
#define ECHO_SERVER_TCPCONNECTION_H

#include <boost/asio.hpp>
#include <vector>

using boost::asio::ip::tcp;
class TCPServer;


class TCPConnection: public std::enable_shared_from_this<TCPConnection> {
    typedef std::shared_ptr<TCPConnection> pointer;
    tcp::socket socket_;
    std::vector<char> buf;
    TCPServer& server;

    TCPConnection(boost::asio::io_context& io_context, TCPServer& server_);
public:
    static pointer create(boost::asio::io_context& io_context, TCPServer& server_) {
        return pointer(new TCPConnection(io_context, server_));
    };
    void start();
    void handle_read();
    void handle_write(const boost::system::error_code&, int bytes_sent);
    void disconnect();
    void count();
    tcp::socket& get_socket();
};
#endif //ECHO_SERVER_TCPCONNECTION_H
