#include <iostream>
#include <vector>
#include <format>
#include "AllocHooker.h"

int main() {
    std::vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    return 0;
}