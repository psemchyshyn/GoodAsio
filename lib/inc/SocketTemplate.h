#ifndef ECHO_SERVER_SOCKETTEMPLATE_H
#define ECHO_SERVER_SOCKETTEMPLATE_H
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include "TimeMeasurement.h"
#include <vector>
#include <algorithm>
#include <set>

class ServerSocket;

class Socket {
protected:
    int fd_;
public:
    explicit Socket(int fd);;

    int get_fd() const;

    virtual ~Socket();

    virtual void sread() =0;

    virtual void swrite(){};
};

#endif //ECHO_SERVER_SOCKETTEMPLATE_H
