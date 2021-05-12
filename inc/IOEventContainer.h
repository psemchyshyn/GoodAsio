//
// Created by msemc on 11.05.2021.
//

#ifndef ECHO_SERVER_IOEVENTCONTAINER_H
#define ECHO_SERVER_IOEVENTCONTAINER_H
#include "Selector.h"
#include "Event.hpp"


class IOEventContainer {
    static const char READ = 'r';
    static const char WRITE = 'w';
    std::vector<IOEvent*> ready;
    std::vector<IOEvent*> container;
    Selector tracker{};

public:
    IOEventContainer() =default; // type epoll_vs select
    template<typename Callback> void append(ReadEvent<Callback>* ev) {
        tracker.register_read_fd(ev->get_socket()->get_fd());
        container.push_back(ev);
    }
    template<typename Callback> void append(WriteEvent<Callback>* ev){
        tracker.register_write_fd(ev->get_socket()->get_fd());
        container.push_back(ev);
    }

    void update_all(int timeout) {
        tracker.update_all(timeout);
        ready.clear();
        std::vector<IOEvent*> temp;
        for (auto e: container){
            auto fd = e->get_socket()->get_fd();
            if ((e->get_type() == READ && tracker.is_set_read(fd)) || (e->get_type() == WRITE && tracker.is_set_write(fd))) {
                e->setReady();
                ready.push_back(e);
            } else {
                temp.push_back(e);
            }
        }
        container = temp;
    }
    std::vector<IOEvent*> get_ready_io_evs() {
        return std::move(ready);
    }

    bool empty() const {
        return ready.empty() && container.empty();
    }
};

#endif //ECHO_SERVER_IOEVENTCONTAINER_H
