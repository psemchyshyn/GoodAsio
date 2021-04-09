//
// Created by msemc on 09.04.2021.
//

#ifndef ECHO_SERVER_EVENT_HPP
#define ECHO_SERVER_EVENT_HPP

class Socket;

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
    Socket *socket_;
public:
    IOEvent(Socket *socket){
        socket_ = socket;
    };
    virtual ~IOEvent() =default;
    virtual void execute() =0;
    virtual Socket* get_socket() {
        return socket_;
    }
};

template<typename Callback>
class ReadEvent: public IOEvent{
    Callback cb_;
public:
    ReadEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() {
        this->socket_->read();
        cb_();
    }
};

template<typename Callback>
class WriteEvent: public IOEvent{
    Callback cb_;
public:
    WriteEvent(Socket *socket, Callback callback): IOEvent{socket}, cb_{callback}{};
    void execute() {
        this->socket_->write();
        cb_();
    }
};


#endif //ECHO_SERVER_EVENT_HPP
