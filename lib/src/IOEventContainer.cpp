#include "../inc/IOEventContainer.h"

IOEventContainer::IOEventContainer(IOManager *tracker_) {
    tracker = tracker_;
}

IOEventContainer::~IOEventContainer() {
    delete tracker;
}

void IOEventContainer::update_all_p(int timeout) {
    auto res = tracker->extract_all(timeout);
    ready = std::vector<IOEvent*>{};
    std::vector<IOEvent*> temp;
    for (auto e: container){
        auto fd = e->get_socket()->get_fd();
        if (res.find(std::pair{fd, e->get_type()}) != res.end()) {
            e->setReady();
            ready.push_back(e);
        } else {
            temp.push_back(e);
        }
    }
//    std::cout << "Container size:" << container.size() << '\n';
//    std::cout << "Ready size:" << ready.size() << '\n';
//    std::cout << "Diff:" << container.size() - ready.size() << '\n';
    container = temp;
}

std::vector<IOEvent *> IOEventContainer::get_ready_io_evs() {
    return ready;
}

bool IOEventContainer::empty() const {
    return ready.empty() && container.empty();
}
