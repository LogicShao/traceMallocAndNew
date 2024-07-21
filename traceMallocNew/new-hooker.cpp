#include "new-hooker.h"
#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib") // 链接DbgHelp.lib

namespace hooker {
	std::map<void*, AllocInfo> allocs;
    // enable默认为false
    // 保证在main函数中调用EnableHook之前，不会记录分配的内存
	bool enable = false;

    void add_AllcInfo(void* ptr, size_t size, void* caller);
    void EnableHook();
    void DisableHook();
    void checkLeaks();

    bool DbgisInitialized = false;
    void InitializeDbgHelp();
    std::string getCallerInfo(void* caller);
}

// 使用inline避免链接时的多重定义问题
inline void* operator new(size_t size) {
    // 获取调用new的位置
    void* pCaller = _ReturnAddress();

    void* ptr = malloc(size); // 分配内存

    // 如果hooker::enable为true，则记录分配的内存
    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        if (ptr != nullptr) {
            printf("new: size=%zu at %p caller=%p\n", size, ptr, pCaller);
            hooker::add_AllcInfo(ptr, size, pCaller);
        }
    }

    // 恢复hooker::enable的值
    hooker::enable = was_enable;

    // 处理分配失败的情况
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    // 标准要求delete nullptr是安全的
    if (ptr == nullptr) return;

    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        printf("delete: %p\n", ptr);
        hooker::allocs.erase(ptr);
    }

    free(ptr);
    hooker::enable = was_enable;
}

// 添加AllocInfo
void hooker::add_AllcInfo(void* ptr, size_t size, void* caller) {
	allocs[ptr] = AllocInfo{ptr, size, caller};
}

// 启用hook
void hooker::EnableHook() {
	enable = true;
}

// 禁用hook
void hooker::DisableHook() {
	enable = false;
}

// 检查内存泄漏
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

// 初始化函数
void hooker::InitializeDbgHelp() {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) {
        printf("SymInitialize failed with error %lu\n", GetLastError());
    }
}

// 用于转化函数地址为代码位置
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