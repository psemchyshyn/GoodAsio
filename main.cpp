#include <iostream>
#include <vector>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/asio/ts/buffer.hpp>


class printer {
    boost::asio::steady_timer timer;
    int count;

public:
    printer(boost::asio::io_context& context):
    timer{context, boost::asio::chrono::seconds(0)},
    count{0}
    {
        timer.async_wait(boost::bind(&printer::print, this));
    }

    void print() {
        count++;
        if (count <= 5) {
            std::cout << count << std::endl;
            timer.expires_at(timer.expiry() + boost::asio::chrono::seconds(1));
            timer.async_wait(boost::bind(&printer::print, this));
        }
    }

    ~printer() {
        std::cout << "Finished" << std::endl;
    }
};



int main() {
    boost::asio::io_context context;
//    boost::asio::steady_timer timer(context, boost::asio::chrono::seconds(5));
    printer pr{context};
//    timer.async_wait(&print);
    context.run();

//    using namespace boost;
//    system::error_code ec;
//    asio::io_context context;
//    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);
//    asio::ip::tcp::socket socket(context);
//
//    socket.connect(endpoint, ec);
//    if (!ec) {
//        std::cout << "Connected successfully" << std::endl;
//    } else {
//        std::cout << "Couldn't connect" << std::endl;
//    }
//    if (socket.is_open()) {
//        std::string request = "GET /index.html HTTP/1.1\r\n"
//                              "Host: example.com\r\n"
//                              "Connection: close\r\n\r\n";
//        socket.write_some(asio::buffer(request.data(), request.size()), ec);
//        socket.wait(socket.wait_read);
//
//        size_t bytes_available = socket.available();
//        if (bytes_available) {
//            std::vector<char> outBuffer(bytes_available);
//            socket.read_some(asio::buffer(outBuffer.data(), bytes_available), ec);
//            for (auto chr: outBuffer) {
//                std::cout << chr;
//            }
//        }
//    }

    return 0;
}
