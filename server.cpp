//
// Created by msemc on 09.03.2021.
//
#include <boost/asio.hpp>
#include <iostream>
#include <vector>


const int BUF_SIZE = 1024;


int main(int argc, char*argv[]) {
    boost::system::error_code error;
    boost::asio::io_context context;

    // Read server configuration - ip address + port to set it up
    const int port = std::stoi(argv[2]);
    auto const address = boost::asio::ip::make_address(argv[1], error);
    if (error) {
        std::cout << error.message() << std::endl;
        return -1;
    }

    // init server
    boost::asio::ip::tcp::endpoint endpoint(address, port);
    boost::asio::ip::tcp::acceptor acceptor{context, endpoint};
    std::cout << "Initialized a server " << argv[1] << ":" << argv[2] << std::endl;


    std::vector<char> buf{};
    // running server
    for (;;) {
        // creates session socket - blocking until connection is made
        boost::asio::ip::tcp::socket session_socket(context);
        acceptor.accept(session_socket);
        session_socket.wait(session_socket.wait_read); // wait until client's request

        int bytes = session_socket.available();
        buf.resize(bytes);
        session_socket.read_some(boost::asio::buffer(buf.data(), bytes), error);
        if (error) {
            std::cout << error.message() << std::endl;
            continue;
        }
        // Print clien't request
        std::cout << "Read: " << bytes << " bytes" << std::endl;
        for (int i = 0; i < bytes; i++) {
            std::cout << buf.at(i);
        }
        // Echo back
        session_socket.write_some(boost::asio::buffer(buf.data(), bytes), error);
        if (error) {
            std::cout << error.message() << std::endl;
            continue;
        }
    }
};