#ifndef ECHO_SERVER_EVENTQUEUE_H
#define ECHO_SERVER_EVENTQUEUE_H
#include <queue>

#include "IOEventContainer.h"

class Compare {
public:
    bool operator()(TimeEvent* left, TimeEvent* right);
};


class EventQueue {
    std::queue<Event *> ready_tasks;
    IOEventContainer io_events{new Epoller{}};
    std::priority_queue<TimeEvent*, std::vector<TimeEvent*>, Compare> timer_events;
public:

    EventQueue() = default;

    template<typename Callback>
    void submit(ClockEvent<Callback> *e) {
        timer_events.push(e);
    }

    template<typename Callback>
    void submit(ReadEvent<Callback> *e) {
        io_events.append(e);
    }

    template<typename Callback>
    void submit(WriteEvent<Callback> *e) {
        io_events.append(e);
    }

    Event* pop();

    void update_ready_tasks();

    bool empty();
};
#endif //ECHO_SERVER_EVENTQUEUE_H
