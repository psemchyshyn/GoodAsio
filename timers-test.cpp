//
// Created by msemc on 11.06.2021.
//
#include <chrono>
#include <thread>
#include "./lib/inc/TimeFunctions.h"


int main() {
    auto callback1 = []() {
        std::cout << "hello" << '\n';
    };
    auto callback2 = []() {
        std::cout << "hd" << '\n';
    };
    auto callback3 = []() {
        std::cout << "new" << '\n';
    };
    EventLoop e{};
    setTime(e, 5, callback3);
    setTime(e, 3, callback1);
    std::cout << "here" << '\n';
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    setTime(e, 1, callback2);
    e.run();
}