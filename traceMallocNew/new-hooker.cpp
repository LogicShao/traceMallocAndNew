#include "new-hooker.h"
#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib") // ����DbgHelp.lib

namespace hooker {
	std::map<void*, AllocInfo> allocs;
    // enableĬ��Ϊfalse
    // ��֤��main�����е���EnableHook֮ǰ�������¼������ڴ�
	bool enable = false;

    void add_AllcInfo(void* ptr, size_t size, void* caller);
    void EnableHook();
    void DisableHook();
    void checkLeaks();

    bool DbgisInitialized = false;
    void InitializeDbgHelp();
    std::string getCallerInfo(void* caller);
}

// ʹ��inline��������ʱ�Ķ��ض�������
inline void* operator new(size_t size) {
    // ��ȡ����new��λ��
    void* pCaller = _ReturnAddress();

    void* ptr = malloc(size); // �����ڴ�

    // ���hooker::enableΪtrue�����¼������ڴ�
    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        if (ptr != nullptr) {
            printf("new: size=%zu at %p caller=%p\n", size, ptr, pCaller);
            hooker::add_AllcInfo(ptr, size, pCaller);
        }
    }

    // �ָ�hooker::enable��ֵ
    hooker::enable = was_enable;

    // �������ʧ�ܵ����
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    // ��׼Ҫ��delete nullptr�ǰ�ȫ��
    if (ptr == nullptr) return;

    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        printf("delete: %p\n", ptr);
        hooker::allocs.erase(ptr);
    }

    free(ptr);
    hooker::enable = was_enable;
}

// ���AllocInfo
void hooker::add_AllcInfo(void* ptr, size_t size, void* caller) {
	allocs[ptr] = AllocInfo{ptr, size, caller};
}

// ����hook
void hooker::EnableHook() {
	enable = true;
}

// ����hook
void hooker::DisableHook() {
	enable = false;
}

// ����ڴ�й©
void hooker::checkLeaks() {
	if (allocs.empty()) {
		printf("No memory leaks.\n");
	} else {
		printf("Memory leaks: {\n");
		for (const auto& [ptr, info] : allocs) {
			printf("  ptr=%p size=%zu caller=%s\n",
                info.ptr, info.size, 
                hooker::getCallerInfo(info.caller).c_str()
            );
		}
        printf("}\n");
	}
}

// ��ʼ������
void hooker::InitializeDbgHelp() {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) {
        printf("SymInitialize failed with error %lu\n", GetLastError());
    }
}

// ����ת��������ַΪ����λ��
std::string hooker::getCallerInfo(void* address) {
    if (hooker::DbgisInitialized == false) {
		hooker::InitializeDbgHelp();
		hooker::DbgisInitialized = true;
	}

    DWORD64 displacement = 0;
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    if (SymFromAddr(GetCurrentProcess(), (DWORD64)address, &displacement, symbol)) {
        return symbol->Name;
    }
    else {
        return "Unknown function";
    }
}