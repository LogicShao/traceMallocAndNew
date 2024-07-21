#include <iostream>
#include "new-hooker.h"

int main() {
	hooker::EnableHook();

	int* p1 = new int;
	short* p2 = new short;
	delete p1;
	delete p2;

	hooker::DisableHook();
	return 0;
}