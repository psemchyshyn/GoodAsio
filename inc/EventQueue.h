#ifndef ECHO_SERVER_EVENTQUEUE_H
#define ECHO_SERVER_EVENTQUEUE_H
#include <queue>
#include "SocketPull.h"
#include "Event.hpp"


class EventQueue {
    SocketPull socket_pull;
    std::queue<Event *> ready_tasks;
    std::vector<IOEvent *> io_events;

public:
    EventQueue() = default;

    template<typename Callback>
    void submit(ReadEvent<Callback> *e) {
        socket_pull.register_read(e->get_socket());
        submit((IOEvent*) e);
    }

    template<typename Callback>
    void submit(WriteEvent<Callback> *e) {
        socket_pull.register_write(e->get_socket());
        submit((IOEvent*)e);
    }

    void submit(IOEvent *e) {
        io_events.push_back(e);
    }

    Event* pop();
    bool empty();
};
#endif //ECHO_SERVER_EVENTQUEUE_H
