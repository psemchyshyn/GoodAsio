#include "../inc/SocketPull.h"


void SocketPull::register_read(Socket *socket) {
    selector.register_read_fd(socket->get_fd());
    sockets.insert(socket);
}

void SocketPull::register_write(Socket *socket) {
    selector.register_write_fd(socket->get_fd());
    sockets.insert(socket);
}

std::set<Socket*> SocketPull::getFreeSockets(int timeout) {
    auto readable_fds = selector.extract_readables(timeout);
    auto writable_fds = selector.extract_writeables(timeout);
    std::set<Socket*> result;
    for (auto s: sockets) {
        if (readable_fds.find(s->get_fd()) != readable_fds.end() || writable_fds.find(s->get_fd()) != writable_fds.end()) {
            result.insert(s);
        }
    }
    sockets.erase(result.begin(), result.end());
    return result; // bug with potential addition to read and write simultaneously
}