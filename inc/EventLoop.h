#ifndef ECHO_SERVER_EVENTLOOP_H
#define ECHO_SERVER_EVENTLOOP_H

#include "EventQueue.h"


class EventLoop {
    EventQueue event_q{};
public:
    template <typename Callback> void addEvent(ReadEvent<Callback>* event) {
        event_q.submit(event);
    }

    template <typename Callback> void addEvent(WriteEvent<Callback>* event) {
        event_q.submit(event);
    }

    void run() {
        while (!event_q.empty()) {
            auto event = event_q.pop(); // finished first
//            std::cout << "print before execute\n";
            event->execute();
        }
    }
};

//class Context {
//    EventLoop ev_loop{};
//public:
//    Context() {};
//};

#endif //ECHO_SERVER_EVENTLOOP_H
