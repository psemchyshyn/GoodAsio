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
    virtual void execute() = 0;
    virtual Socket* get_socket() {
        return socket_;
    }

    // Добавив цю функцію бо вона фіксить помилку з е->setReady() в EventQueue::pop
    void setReady() override {
        ready = true;
    }
};

template<typename Callback>
class ReadEvent: public IOEvent{
    Callback cb_;
public:
    ReadEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() final{
        this->socket_->sread();
        cb_();
    }
};

template<typename Callback>
class WriteEvent: public IOEvent{
    Callback cb_;
public:
    WriteEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() final {
        this->socket_->swrite();
        cb_();
    }
};

//template<typename Callback>
//class AcceptEvent: public ReadEvent<Callback> {
//public:
//    AcceptEvent(Server)
//};
#endif //ECHO_SERVER_EVENT_HPP
