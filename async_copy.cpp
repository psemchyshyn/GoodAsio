//
// Created by msemc on 20.03.2021.
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}


class tcp_connection: public boost::enable_shared_from_this<tcp_connection> {
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_context& io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start() {
        message_ = make_daytime_string();
        boost::asio::async_write(socket_, boost::asio::buffer(message_),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
private:
    explicit tcp_connection(boost::asio::io_context& io_context)
            : socket_(io_context)
    {
    }

    void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/)
    {
        std::cout << "Sended to client" << std::endl;
    }

    tcp::socket socket_;
    std::string message_;
};


class tcp_server {
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
public:
    explicit tcp_server(boost::asio::io_context &io_context):
            io_context_(io_context),
            acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 80))
    {
        start_accept();
    }

private:
    void start_accept() {
        tcp_connection::pointer new_connection =
                tcp_connection::create(io_context_);
    }

    void handle_accept(tcp_connection::pointer new_connection,
                       const boost::system::error_code &error) {
        if (!error) {
            new_connection->start();
        }


        start_accept();
    }
};

int main()
{
    try {
        boost::asio::io_context io_context;
        tcp_server server(io_context);
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}