#include "../inc/IOManager.h"


void Selector::register_read_fd(int fd) {
    if (fd > max_descriptor) {
        max_descriptor = fd;
    }
    tracking.push_back(std::pair{fd, 'r'});
    FD_SET(fd, &master_read_fds);
}

void Selector::register_write_fd(int fd) {
    if (fd > max_descriptor) {
        max_descriptor = fd;
    }
    tracking.push_back(std::pair{fd, 'w'});
    FD_SET(fd, &master_write_fds);
}

std::set<std::pair<int, char>> Selector::extract_all(int timeout) {
    fd_set reads_alive_set = master_read_fds;
    fd_set writes_alive_set = master_write_fds; // later reads_set will be cleared of those descriptors that can't read
    struct timeval t{0, timeout};
    if (select(max_descriptor + 1, &reads_alive_set, &writes_alive_set, 0, &t) < 0) {
        throw SelectError{"Error in select"};
    }

    std::set<std::pair<int, char>> result;
    for (size_t i = 0; i < tracking.size(); i++) {
        if (tracking.at(i).second == 'r' && FD_ISSET(tracking.at(i).first, &reads_alive_set)) {
            result.insert(tracking.at(i));
            FD_CLR(tracking.at(i).first, &master_read_fds);
        } else if (tracking.at(i).second == 'w' && FD_ISSET(tracking.at(i).first, &writes_alive_set)) {
            result.insert(tracking.at(i));
            FD_CLR(tracking.at(i).first, &master_write_fds);
        }
    }

    tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&result](std::pair<int, char> e) {
        return result.find(e) != result.end();
    }), tracking.end());
    return result;
}

void Poller::register_read_fd(int fd) {
    struct pollfd curr;
    curr.fd = fd;
    curr.events = POLLIN;
    tracking.push_back(curr);
}

void Poller::register_write_fd(int fd) {
    struct pollfd curr;
    curr.fd = fd;
    curr.events = POLLOUT;
    tracking.push_back(curr);
}

std::set<std::pair<int, char>> Poller::extract_all(int timeout) {
    if (poll(tracking.data(), tracking.size(), timeout) == -1) {
        throw PollError{"error in poll"};
    }
    std::set<std::pair<int, char>> result;
    for (size_t i = 0; i < tracking.size(); i++) {
        if (tracking.at(i).revents & POLLIN) {
            result.insert(std::pair{tracking.at(i).fd, 'r'});
        } else if (tracking.at(i).revents & POLLOUT) {
            result.insert(std::pair{tracking.at(i).fd, 'w'});
        }
    }
    tracking.erase(std::remove_if(tracking.begin(), tracking.end(), [&result](struct pollfd e) {
        return result.find(std::pair{e.fd, 'r'}) != result.end() || result.find(std::pair{e.fd, 'w'}) != result.end();
    }), tracking.end());
    return result;
}

Epoller::Epoller() : IOManager() {
    epoll_instance_fd = epoll_create1(0);
}

void Epoller::register_read_fd(int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, fd, &ev);
}

void Epoller::register_write_fd(int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLOUT;
    epoll_ctl(epoll_instance_fd, EPOLL_CTL_ADD, fd, &ev);
}

std::set<std::pair<int, char>> Epoller::extract_all(int timeout) {
    auto ready_num = epoll_wait(epoll_instance_fd, events, EVENTS_SIZE, timeout);
    if (ready_num == -1) {
        throw "Error while polling";
    }
    std::set<std::pair<int, char>> result;
    for (size_t i = 0; i < ready_num; i++) {
        int desc = events[i].data.fd;
        if (events[i].events == POLLIN) {
            result.insert(std::pair{desc, 'r'});
        } else if (events[i].events == POLLOUT) {
            result.insert(std::pair{desc, 'w'});
        }
        epoll_ctl(epoll_instance_fd, EPOLL_CTL_DEL, desc, &events[i]);
    }
    return result;
}
