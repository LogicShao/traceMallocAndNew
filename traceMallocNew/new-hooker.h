#ifndef OPERATOR_NEW_HOOKER_CPP
#define OPERATOR_NEW_HOOKER_CPP
#endif

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <new> // 包含标准的new头文件
#include <map>

namespace hooker {
    // 用于记录new/delete的调用
    extern std::map<void*, size_t> allocs;
    // 用于控制是否启用hook
    extern bool enable;

    void EnableHook(); // 启用hook
    void DisableHook(); // 禁用hook
}

// 使用inline避免链接时的多重定义问题
inline void* operator new(size_t size); // hook new
inline void operator delete(void* ptr) noexcept; // hook delete

#undef OPERATOR_NEW_HOOKER_CPP
