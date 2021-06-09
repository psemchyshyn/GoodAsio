#ifndef ECHO_SERVER_IOEVENTCONTAINER_H
#define ECHO_SERVER_IOEVENTCONTAINER_H
#include "IOManager.h"
#include "Event.h"

class IOEventContainer {
    static const char READ = 'r';
    static const char WRITE = 'w';
    std::vector<IOEvent*> ready;
    std::vector<IOEvent*> container;
    IOManager* tracker;
public:
    IOEventContainer(IOManager* tracker_);; // type epoll_vs select

    ~IOEventContainer();

    template<typename Callback>
    void append(ReadEvent<Callback> *ev) {
        tracker->register_read_fd(ev->get_socket()->get_fd());
        container.push_back(ev);
    }

    template<typename Callback>
    void append(WriteEvent<Callback> *ev) {
        tracker->register_write_fd(ev->get_socket()->get_fd());
        container.push_back(ev);
    }
    void update_all_p(int timeout);

    std::vector<IOEvent*> get_ready_io_evs();

    bool empty() const;
};

#endif //ECHO_SERVER_IOEVENTCONTAINER_H
