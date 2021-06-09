#include "../inc/Event.h"

bool Event::isReady() {
    return ready;
}

void Event::setReady() {
    ready = true;
}

IOEvent::IOEvent(Socket *socket) {
    socket_ = socket;
}

Socket *IOEvent::get_socket() {
    return socket_;
}

long TimeEvent::timeLeft() {
    return 0;
}
