#include "../inc/EventLoop.h"

void EventLoop::run() {
    while (!event_q.empty()) {
        event_q.update_ready_tasks();
        auto event = event_q.pop();
        if(event != nullptr) {
            event->execute();
        }
    }
}