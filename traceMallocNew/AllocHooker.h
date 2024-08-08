#ifndef ALLOC_HOOKER_H
#define ALLOC_HOOKER_H

#include "Global_Info.h"

// 使用inline避免链接时的多重定义问题
inline void* operator new(size_t size) {
    void* ptr = malloc(size); // 分配内存

    if (ptr == nullptr) { // 分配失败
        throw std::bad_alloc();
    }

    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onAllocate(
            Global_Info::alloc_op::New,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

inline void* operator new[](size_t size) {
    void* ptr = malloc(size); // 分配内存

    if (ptr == nullptr) { // 分配失败
        throw std::bad_alloc();
    }

    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onAllocate(
            Global_Info::alloc_op::New_Array,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

inline void* operator new(size_t size, const std::nothrow_t&) noexcept {
    void* ptr = malloc(size); // 分配内存
    // 没有抛出异常的版本，分配失败返回nullptr
    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onAllocate(
            Global_Info::alloc_op::New,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    void* ptr = malloc(size); // 分配内存

    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onAllocate(
            Global_Info::alloc_op::New_Array,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onDeallocate(
            Global_Info::alloc_op::Delete,
            ptr, _ReturnAddress()
        );
    }
}

inline void operator delete[](void* ptr) noexcept {
    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onDeallocate(
            Global_Info::alloc_op::Delete_Array,
            ptr, _ReturnAddress()
        );
    }
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onDeallocate(
            Global_Info::alloc_op::Delete,
            ptr, _ReturnAddress()
        );
    }
}

inline void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    Global_Info::EnableGuard guard;
    if (guard) {
        Global_Info::getGD().onDeallocate(
            Global_Info::alloc_op::Delete_Array,
            ptr, _ReturnAddress()
        );
    }
}

#undef ALLOC_HOOKER_H
#endif
