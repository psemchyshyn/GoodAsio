#ifndef ECHO_SERVER_EVENT_HPP
#define ECHO_SERVER_EVENT_HPP

#include "SocketTemplate.h"

class Event {
protected:
    bool ready = false;
public:
    virtual ~Event() =default;
    virtual void execute() =0;
    virtual bool isReady() {
        return ready;
    };

    virtual void setReady() {
        ready = true;
    };
};

class IOEvent: public Event {
protected:
    Socket* socket_;
public:
    IOEvent(Socket *socket){
        socket_ = socket;
    };
    virtual ~IOEvent() =default;
    virtual void execute() =0;
    virtual Socket* get_socket() {
        return socket_;
    }

    virtual char get_type() =0;
};

template<typename Callback>
class ReadEvent: public IOEvent{
    Callback cb_;
public:
    ReadEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() override {
        this->socket_->sread();
        cb_();
    }

    char get_type() override {
        return 'r';
    }
};

template<typename Callback>
class WriteEvent: public IOEvent{
    Callback cb_;
public:
    WriteEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() override {
        this->socket_->swrite();
        cb_();
    }

    char get_type() override {
        return 'w';
    }
};

class TimeEvent: public Event {
public:
    virtual long timeLeft(){
        return 0;
    };
};

template<typename Callback>
class ClockEvent: public TimeEvent{
    std::chrono::high_resolution_clock::time_point begin;
    int wait_for;
    Callback cb_;
public:
    ClockEvent(int sec, Callback callback): wait_for{sec}, cb_{callback}, begin{get_current_time()}{};

    void execute() override {
        cb_();
    }

    bool isReady() override {
        return to_sec(get_current_time() - begin) >= wait_for;
    }

    long timeLeft() override {
        long time = wait_for - to_sec(get_current_time() - begin);
        return time > 0 ? time : 0;
    }
};
#endif //ECHO_SERVER_EVENT_HPP
