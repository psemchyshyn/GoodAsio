#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include "my_time.h"
#include <string>
#include <fstream>
#include <filesystem>


int main(int argc, char*argv[]) {
    boost::system::error_code error;
    boost::asio::io_context context;
    std::cout << argv[1] << ":" << argv[2] << std::endl;

    auto const address = boost::asio::ip::make_address(argv[1], error);
    if (error) {
        std::cout << error.message() << std::endl;
        return -1;
    }
    const int port = std::stoi(argv[2]);


    boost::asio::ip::tcp::endpoint endpoint(address, port);
    boost::asio::ip::tcp::acceptor acceptor{context, endpoint};
    std::vector<int> v_conn_per_sec{};
    std::string response{"Client's request"};
    int alive_connections;

    std::vector<char> buf{};

    std::ofstream f{"../result.txt"};

    auto start = get_current_time();
    for (;;) {
        boost::asio::ip::tcp::socket session_socket(context);
        acceptor.accept(session_socket); // accepted connection

        session_socket.wait(session_socket.wait_read);
        int bytes = session_socket.available();
        buf.resize(bytes);
        session_socket.read_some(boost::asio::buffer(buf.data(), bytes), error);

        if (error) {
            std::cout << error.message() << std::endl;
            continue;
        }

        if (std::string(buf.begin(), buf.end()) == response) {
            alive_connections++;
        }
        session_socket.write_some(boost::asio::buffer(response.data(), response.size()), error);

        // measure time
        auto now = get_current_time() - start;
        if (to_us(now) > 1000000) {
            f.close();
            f = std::ofstream{"../result.txt"};
            f << alive_connections << std::endl;
            start = get_current_time();
            v_conn_per_sec.push_back(alive_connections);
            std::cout << alive_connections << std::endl;
            alive_connections = 0;
        }
    }
    return 0;
};