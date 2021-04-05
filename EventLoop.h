//
// Created by msemc on 04.04.2021.
//

#ifndef ECHO_SERVER_EVENTLOOP_H
#define ECHO_SERVER_EVENTLOOP_H

#include <queue>
#include "Selector.h"
#include <functional>
#include <unordered_map>

class EventQueue {
    const int BUFSIZE = 20;
    Selector selector{};
    std::queue<std::function<void()>> ready_tasks;
    std::unordered_map<int, *Event> io_events;

public:
    EventQueue(){};
    void submit(ReadEvent *e);
    void submit(WriteEvent *e);
    std::function<void()> pop(){
        if (!ready_tasks.empty()) {
            auto callback = ready_tasks.front();
            ready_tasks.pop();
            return callback;
        }

        for (auto fd: selector.extract_readables(1)) {
            std::string buf(BUFSIZE, ' ');
            int available = read(fd, buf.data(), BUFSIZE);
            if (available < 0)
                throw "Read went wrong"; // will change later
            ready_tasks.push(io_events.at(fd).get_callback()); // + buffer to add
        }

        for (auto fd: selector.extract_writeables(1)) {
            int written = write(i, buf, strlen(buf));
            if (written < 0)
                error("ERROR writing to socket");
            ready_tasks.push(io_events.at(fd).get_callback()); // + buffer to add
        }

        if (ready_tasks.empty()){
            return pop();
        } else {
            auto callback = ready_tasks.front();
            ready_tasks.pop();
            return callback;
        }
    }
};

class EventLoop {
    std::queue<int> event_q; //custom queue
    Selector selector{};
public:
    void addEvent(Event* event) {

    }

    void run() {
        while (!event_q.empty()) {
            auto event = event_q.pop(); // finished first
            event.execute();
        }
    }
};

class Context {
    EventLoop ev_loop{};
public:
    Context() {};
};

#endif //ECHO_SERVER_EVENTLOOP_H
