#include <iostream>
#include "new-hooker.h"

static char* newtest() {
    char* p = new char;
    return p;
}

int main() {
    int* p1 = new int;
    short* p2 = new short;
    char* p3 = newtest();
    delete p1;
    return 0;
}