#include "../inc/EventLoop.h"

void EventLoop::run() {
    while (!event_q.empty()) {
        auto event = event_q.pop();
        if(event != nullptr) {
            event->execute();
            delete event;
        } else {
            event_q.update_ready_tasks();
        }
    }
}