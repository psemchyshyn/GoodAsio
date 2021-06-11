#ifndef ECHO_SERVER_EVENT_H
#define ECHO_SERVER_EVENT_H
#include "SocketTemplate.h"

class Event {
protected:
    bool ready = false;
public:
    virtual ~Event() =default;

    virtual void execute() =0;

    virtual bool isReady();

    virtual void setReady();
};

class IOEvent: public Event {
protected:
    Socket* socket_;
public:
    IOEvent(Socket *socket);;

    virtual ~IOEvent() =default;

    virtual void execute() =0;

    virtual Socket* get_socket();

    virtual char get_type() =0;
};

class TimeEvent: public Event {
public:
    virtual long timeLeft();
    virtual ~TimeEvent() =default;
};

template<typename Callback>
class ReadEvent: public IOEvent{
    Callback cb_;
public:
    ReadEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{}

    void execute() {
        this->socket_->sread();
        cb_();
    }

    char get_type() {
        return 'r';
    }

    ~ReadEvent() =default;
};

template<typename Callback>
class WriteEvent: public IOEvent{
    Callback cb_;
public:
    WriteEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{}

    void execute() {
        this->socket_->swrite();
        cb_();
    }

    char get_type() {
        return 'w';
    }

    ~WriteEvent() =default;
};

template<typename Callback>
class ClockEvent: public TimeEvent{
    std::chrono::high_resolution_clock::time_point begin;
    int wait_for;
    Callback cb_;
public:
    ClockEvent(int sec, Callback callback): wait_for{sec}, cb_{callback}, begin{get_current_time()}{}

    void execute() {
        cb_();
    }

    bool isReady() {
        return to_sec(get_current_time() - begin) >= wait_for;
    }

    long timeLeft() {
        long time = wait_for - to_sec(get_current_time() - begin);
        return time > 0 ? time : 0;
    }

    ~ClockEvent() =default;
};
#endif //ECHO_SERVER_EVENT_H
