#include "../inc/SocketTemplate.h"

Socket::Socket(int fd) : fd_(fd){}

int Socket::get_fd() const {
    return fd_;
}

Socket::~Socket() {
//        std::cout << "Closed: " << fd_ << '\n';
    close(fd_);
}
