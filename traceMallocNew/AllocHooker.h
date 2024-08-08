#ifndef ALLOC_HOOKER_H
#define ALLOC_HOOKER_H

#include "Global_Info.h"

// ʹ��inline��������ʱ�Ķ��ض�������
inline void* operator new(size_t size) {
    void* ptr = malloc(size); // �����ڴ�

    if (ptr == nullptr) { // ����ʧ��
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
    void* ptr = malloc(size); // �����ڴ�

    if (ptr == nullptr) { // ����ʧ��
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
    void* ptr = malloc(size); // �����ڴ�
    // û���׳��쳣�İ汾������ʧ�ܷ���nullptr
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
    void* ptr = malloc(size); // �����ڴ�

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
