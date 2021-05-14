//
// Created by msemc on 13.05.2021.
//

#include <set>
#include <iostream>
int main() {
    std::set<std::pair<int, char>> set;
    std::pair<int, char> a{4, 'f'};
    set.insert(a);
    std::pair<int, char> b{4, 'f'};
    std::cout << (set.find(b)->first);

    return 0;
}