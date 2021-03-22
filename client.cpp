//
// Created by msemc on 09.03.2021.
//

#include <boost/asio.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    boost::system::error_code error;
    boost::asio::io_context context;

    // Read configuration to which server to connect
    const int port = std::stoi(argv[2]);
    auto const address = boost::asio::ip::make_address(argv[1], error);
    if (error) {
        std::cout << error.message() << std::endl;
        return -1;
    }

    boost::asio::ip::tcp::endpoint server_endpoint(address, port); // server
    boost::asio::ip::tcp::socket client_socket(context);
    client_socket.connect(server_endpoint, error);
    if (error) {
        std::cout << error.message() << std::endl;
        return -1;
    }
    std::cout << "Connected to: " << argv[1] << ":" << argv[2] << std::endl;

    if (client_socket.is_open()) {
//        std::string request{"Client's request"};
//        client_socket.write_some(boost::asio::buffer(request.data(), request.size()), error);
//        if (error) {
//            std::cout << error.message() << std::endl;
//            return -1;
//        }
        client_socket.wait(client_socket.wait_read);
        int resp_size = client_socket.available();

        if (resp_size) {
            std::vector<char> buf{};
            buf.resize(resp_size);
            client_socket.read_some(boost::asio::buffer(buf, resp_size), error);
            if (error) {
                std::cout << error.message() << std::endl;
            }
            for (char chr: buf) {
                std::cout << chr;
            }
        }
    }
}