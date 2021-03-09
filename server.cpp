//
// Created by msemc on 09.03.2021.
//
#include <boost/asio.hpp>
#include <iostream>
#include <vector>


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

    std::vector<char> buf(1024);
    for (;;) {
        boost::asio::ip::tcp::socket session_socket(context);
        acceptor.accept(session_socket);
        session_socket.wait(session_socket.wait_read);
        int bytes = session_socket.available();
        session_socket.read_some(boost::asio::buffer(buf.data(), bytes), error);
        std::cout << "Read: " << bytes << " bytes" << std::endl;

        for (char chr: buf) {
            std::cout << chr;
        }

        std::string response{"Server's response"};
        session_socket.write_some(boost::asio::buffer(response.data(), response.size()), error);

    }
};