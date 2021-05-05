#include "../inc/EventQueue.h"

Event* EventQueue::pop() {
    while(true) {
        if (!ready_tasks.empty()) {
            Event* e = ready_tasks.front();
            ready_tasks.pop();
//            std::cout << "event returned\n";
            return e;
        }

        auto available_sockets = socket_pull.getFreeSockets(1); // change later
        std::vector<IOEvent*> remaining;
        for (auto e: io_events) {
            if (std::find(available_sockets.begin(), available_sockets.end(), e->get_socket()) != available_sockets.end()) {
//                if (available_sockets.find(e->get_socket()) != available_sockets.end()) {
                e->setReady();
//                std::cout << "event added\n"; // тут прінтиться
                ready_tasks.push(e);

                // change later
//                for (auto i = io_events.begin(); i != io_events.end(); i++) {
//                    if (*i == e) {
//
//                    }
//                }
//                std::cout << "event erased\n";
            } else {
                remaining.push_back(e);
            }
        }
        io_events = remaining;
    }
}

bool EventQueue::empty() {
    return io_events.empty() && ready_tasks.empty();
}
