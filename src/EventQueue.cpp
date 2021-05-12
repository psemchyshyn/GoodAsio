#include "../inc/EventQueue.h"

Event* EventQueue::pop() {
    while(true) {
        if (!ready_tasks.empty()) {
            Event* e = ready_tasks.front();
            ready_tasks.pop();
//            std::cout << "event returned\n";
            return e;
        } else {
            io_events.update_all(1);
            auto ready_io = io_events.get_ready_io_evs();
            while (!ready_io.empty()) {
                ready_tasks.push(ready_io.front());
                ready_io.pop_back();
            }
        }
    }
}

bool EventQueue::empty() {
    return io_events.empty() && ready_tasks.empty();
}
