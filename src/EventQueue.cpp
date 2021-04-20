#include "../inc/EventQueue.h"

Event* EventQueue::pop() {
    if (!ready_tasks.empty()) {
        Event* e = ready_tasks.front();
        ready_tasks.pop();
        return e;
    }

while(true) {
        auto available_sockets = socket_pull.getFreeSockets(5); // change later
        for (auto e: io_events) {
            if (available_sockets.find(e->get_socket()) != available_sockets.end()) {
                e->setReady();
                ready_tasks.push(e);
            }
        }
        if (!ready_tasks.empty()) {
            Event* e = ready_tasks.front();
            ready_tasks.pop();
            return e;
        }
    }
}

bool EventQueue::empty() {
    return io_events.empty() && ready_tasks.empty();
}
