#include <iostream>
#include "new-hooker.h"

int* test(int x) {
	int* p = new int;
	return p;
}

int main() {
	hooker::EnableHook();

	int* p1 = new int;
	short* p2 = new short;
	int *p3 = test(10);
	delete p1;
	delete p2;

	hooker::DisableHook();
	hooker::checkLeaks();
	return 0;
}