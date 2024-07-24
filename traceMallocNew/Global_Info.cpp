#include "Global_Info.h"

namespace Global_Info {
    std::map<void*, AllocInfo> allocs;
    GlobalData globalData;
    bool enable = false;
}

Global_Info::GlobalData::GlobalData() {
    InitializeDbgHelp(); // 初始化DbgHelp
    EnableHook(); // 启用hook
}

Global_Info::GlobalData::~GlobalData() {
    DisableHook(); // 禁用hook
    checkLeaks(); // 检查内存泄漏
    SymCleanup(GetCurrentProcess()); // 清理SymInitialize
}

// 用于检查allocs是否匹配
bool Global_Info::checkAllocsMatch(alloc_op op1, alloc_op op2) {
    return (static_cast<int>(op1) ^ 1) == static_cast<int>(op2);
}

// 添加AllocInfo
bool Global_Info::add_AllcInfo(
    alloc_op op,
    void* ptr,
    size_t size,
    void* caller
) {
    return allocs.emplace(
        ptr, AllocInfo{ op, ptr, size, caller}
    ).second;
}

// 启用hook
void Global_Info::EnableHook() {
    enable = true;
}

// 禁用hook
void Global_Info::DisableHook() {
    enable = false;
}

// 检查内存泄漏
void Global_Info::checkLeaks() {
    if (allocs.empty()) {
        printf("No memory leaks.\n");
    }
    else {
        printf("Memory leaks: {\n");
        for (const auto& [ptr, info] : allocs) {
            printf("  ptr=%p size=%zu caller=%s\n",
                info.ptr, info.size,
                Global_Info::getCallerInfo(info.caller).c_str()
            );
        }
        printf("}\n");
    }
}

// 初始化函数
void Global_Info::InitializeDbgHelp() {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) {
        printf("SymInitialize failed with error %lu\n", GetLastError());
    }
}

// 用于转化函数地址为代码位置
std::string Global_Info::getCallerInfo(void* address) {
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

Global_Info::EnableGuard::EnableGuard() {
    was_enable = std::exchange(Global_Info::enable, false);
}

Global_Info::EnableGuard::~EnableGuard() {
    Global_Info::enable = was_enable;
}

Global_Info::EnableGuard::operator bool() const {
    return was_enable;
}

void Global_Info::GlobalData::onAllocate(
    alloc_op op,
    void* ptr,
    size_t size,
    void* caller
) {
    // 处理分配失败的情况
    if (ptr == nullptr) {
        printf("%s failed [size=%zu caller=%s]\n",
            alloc_op_str[static_cast<size_t>(op)],
            size, getCallerInfo(caller).c_str()
        );
        return;
    }
    // 处理分配成功的情况
    if (add_AllcInfo(op, ptr, size, caller)) {
        printf("%s [ptr=%p size=%zu caller=%s]\n",
            alloc_op_str[static_cast<size_t>(op)],
            ptr, size, getCallerInfo(caller).c_str()
        );
        return;
    }
    // 处理重复分配的情况
    printf("The same memory address is assigned multiple times: ptr=%p\n", ptr);
}

void Global_Info::GlobalData::onDeallocate(
    alloc_op op,
    void* ptr,
    void* caller
) {
    // 处理删除nullptr的情况
    if (ptr == nullptr) {
        printf("%s failed [try to delete nullptr!]\n",
            alloc_op_str[static_cast<size_t>(op)]
        );
        return;
    }

    // 处理删除未分配的内存的情况
    if (allocs.count(ptr) == 0) {
        printf("%s failed [ptr=%p not found]\n",
            alloc_op_str[static_cast<size_t>(op)], ptr
        );
        return;
    }

    // 处理错误调用的情况
    if (not checkAllocsMatch(allocs[ptr].op, op)) {
        printf("%s failed [ptr=%p alloc by (%s)]\n",
            alloc_op_str[static_cast<size_t>(op)], ptr,
            alloc_op_str[static_cast<size_t>(allocs[ptr].op)]
        );
        return;
    }

    // 处理正常删除的情况
    allocs.erase(ptr);
    printf("%s [ptr=%p caller=%s]\n",
        alloc_op_str[static_cast<size_t>(op)], ptr,
        getCallerInfo(caller).c_str()
    );
}