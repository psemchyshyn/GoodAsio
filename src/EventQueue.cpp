#include "../inc/EventQueue.h"

Event* EventQueue::pop() {
    while(true) {

        if (!ready_tasks.empty()) {
            Event* e = ready_tasks.front();
            ready_tasks.pop();
            std::cout << "event returned\n";
            return e;
        }

        auto available_sockets = socket_pull.getFreeSockets(5); // change later
        for (auto e: io_events) {
            if (available_sockets.find(e->get_socket()) != available_sockets.end()) {
                e->setReady();
                std::cout << "event added\n"; // тут прінтиться
                ready_tasks.push(e);
            }
        }
    }
}

bool EventQueue::empty() {
    return io_events.empty() && ready_tasks.empty();
}
