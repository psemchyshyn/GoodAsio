#ifndef ECHO_SERVER_EVENTQUEUE_H
#define ECHO_SERVER_EVENTQUEUE_H
#include <queue>

#include "IOEventContainer.h"


class EventQueue {
    std::queue<Event *> ready_tasks;
    IOEventContainer io_events;

public:
    EventQueue() = default;

    template<typename Callback>
    void submit(ReadEvent<Callback> *e) {
        io_events.append(e);
    }

    template<typename Callback>
    void submit(WriteEvent<Callback> *e) {
        io_events.append(e);
    }

    Event* pop();
    bool empty();
};
#endif //ECHO_SERVER_EVENTQUEUE_H
