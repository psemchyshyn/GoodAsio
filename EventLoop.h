//
// Created by msemc on 04.04.2021.
//

#ifndef ECHO_SERVER_EVENTLOOP_H
#define ECHO_SERVER_EVENTLOOP_H

#include <queue>



class EventLoop {
    std::queue<Event> event_q;
public:
    void addEvent(Event* event);
    void run();
};


#endif //ECHO_SERVER_EVENTLOOP_H
