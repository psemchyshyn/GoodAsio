#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include "my_time.h"
#include <string>
#include <numeric>


long long read_time(boost::asio::ip::tcp::socket& session_socket, std::vector<char>& buf, boost::system::error_code& error) {
    auto start_acc = get_current_time();
    session_socket.wait(session_socket.wait_read);
    int bytes = session_socket.available();
    session_socket.read_some(boost::asio::buffer(buf.data(), bytes), error);
    auto total_acc = get_current_time() - start_acc;
    return to_us(total_acc);
}

long long write_time(boost::asio::ip::tcp::socket& session_socket, boost::system::error_code& error) {
    std::string response{"Server's response\n"};
    auto start = get_current_time();
    session_socket.write_some(boost::asio::buffer(response.data(), response.size()), error);
    auto total = get_current_time() - start;
    return to_us(total);
}

int main(int argc, char*argv[]) {
    boost::system::error_code error;
    boost::asio::io_context context;
//    std::cout << argv[1] << ":" << argv[2] << std::endl;

    auto const address = boost::asio::ip::make_address(argv[1], error);
    if (error) {
        std::cout << error.message() << std::endl;
        return -1;
    }
    const int port = std::stoi(argv[2]);



    boost::asio::ip::tcp::endpoint endpoint(address, port);

    boost::asio::ip::tcp::acceptor acceptor{context, endpoint};

    std::vector<long long> read_vec{};
    std::vector<long long> write_vec{};

    std::vector<char> buf(1024);
    for (;;) {
        boost::asio::ip::tcp::socket session_socket(context);
        acceptor.accept(session_socket);
        auto read_t = read_time(session_socket, buf, error);
        read_vec.push_back(read_t);
//        std::cout << "Time of accept and read is: " << read_t << "\n";
//        for (char chr: buf) {
//            std::cout << chr;
//        }

        auto write_t = write_time(session_socket, error);
        write_vec.push_back(write_t);
//        std::cout << "\nTime of write is: " << write_t << "\n";
        if (read_vec.size() == 10000) {
            break;
        }
    }
    std::cout <<  std::accumulate(read_vec.begin(), read_vec.end(), 0) << "\n";
    std::cout <<  std::accumulate(write_vec.begin(), write_vec.end(), 0) << "\n";

    return 0;
};