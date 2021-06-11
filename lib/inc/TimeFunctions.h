//
// Created by msemc on 11.06.2021.
//

#ifndef ECHO_SERVER_TIMEFUNCTIONS_H
#define ECHO_SERVER_TIMEFUNCTIONS_H
#include "./EventLoop.h"

template<typename Callback> void setTime(EventLoop &e, int timeout, Callback cb) {
    e.addEvent(new ClockEvent{timeout, cb});
}


#endif //ECHO_SERVER_TIMEFUNCTIONS_H
