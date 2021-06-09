#include "../inc/EventQueue.h"

Event* EventQueue::pop() {
    while(true) {
        if (!ready_tasks.empty()) {
            Event* e = ready_tasks.front();
            ready_tasks.pop();
//            std::cout << "event returned\n";
            return e;
        } else {
            io_events.update_all_p(1);
            auto ready_io = io_events.get_ready_io_evs();
            while (!ready_io.empty()) {
                ready_tasks.push(ready_io.back());
                ready_io.pop_back();
            }
//            while (!timer_events.empty() && timer_events.top()->isReady()){
//                ready_tasks.push(timer_events.top());
//                timer_events.pop();
//            }
        }
    }
}

bool EventQueue::empty() {
    return io_events.empty() && ready_tasks.empty() && timer_events.empty();
}
