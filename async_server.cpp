#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "my_time.h"
#include "thread"

using boost::asio::ip::tcp;


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
//        auto start = get_current_time();
        socket_.wait(socket_.wait_read);
        int bytes = socket_.available();
        buf.resize(bytes);
//        std::cout << bytes << "\n";
        boost::asio::async_read(socket_, boost::asio::buffer(buf.data(), buf.size()), boost::bind(
                &tcp_connection::handle_read, shared_from_this()
                ));

//        auto total = get_current_time() - start;
//        std::cout << "Time of async write is: " << to_us(total) << "\n";
    }
private:
    explicit tcp_connection(boost::asio::io_context& io_context)
            : socket_(io_context)
    {
    }

    void handle_write(const boost::system::error_code& /*error*/,
                      size_t /*bytes_transferred*/)
    {
        std::cout << "Sent!\n";
    }

    void handle_read() {
        for (auto ch : buf) {
            std::cout << ch;
        }


        boost::asio::async_write(socket_, boost::asio::buffer(buf.data(), buf.size()),
                                 boost::bind(&tcp_connection::handle_write, shared_from_this(),
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

    tcp::socket socket_;
    std::vector<char> buf;
};


class tcp_server {
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
public:
    explicit tcp_server(boost::asio::io_context &io_context):
            io_context_(io_context),
            acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1024))
    {
        start_accept();
    }

private:
    void start_accept() {
        tcp_connection::pointer new_connection =
                tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
                               boost::bind(&tcp_server::handle_accept, this, new_connection,
                                           boost::asio::placeholders::error));
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
//        std::thread t(boost::bind(&boost::asio::io_context::run, &io_context));
//        t.join();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
