#include "Global_Info.h"

void* operator new(size_t size) {
    void* ptr = malloc(size); // �����ڴ�

    if (ptr == nullptr) { // ����ʧ��
        throw std::bad_alloc();
    }

    GI::EnableGuard guard;
    if (guard) {
        GI::global.onAllocate(
            GI::alloc_op::New,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

void* operator new[](size_t size) {
    void* ptr = malloc(size); // �����ڴ�

    if (ptr == nullptr) { // ����ʧ��
        throw std::bad_alloc();
    }

    GI::EnableGuard guard;
    if (guard) {
        GI::global.onAllocate(
            GI::alloc_op::New_Array,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

void* operator new(size_t size, const std::nothrow_t&) noexcept {
    void* ptr = malloc(size); // �����ڴ�
    // û���׳��쳣�İ汾������ʧ�ܷ���nullptr
    GI::EnableGuard guard;
    if (guard) {
        GI::global.onAllocate(
            GI::alloc_op::New,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    void* ptr = malloc(size); // �����ڴ�

    GI::EnableGuard guard;
    if (guard) {
        GI::global.onAllocate(
            GI::alloc_op::New_Array,
            ptr, size, _ReturnAddress()
        );
    }

    return ptr;
}

void operator delete(void* ptr) noexcept {
    GI::EnableGuard guard;
    if (guard) {
        GI::global.onDeallocate(
            GI::alloc_op::Delete,
            ptr, _ReturnAddress()
        );
    }
}

void operator delete[](void* ptr) noexcept {
    GI::EnableGuard guard;
    if (guard) {
        GI::global.onDeallocate(
            GI::alloc_op::Delete_Array,
            ptr, _ReturnAddress()
        );
    }
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    GI::EnableGuard guard;
    if (guard) {
        GI::global.onDeallocate(
            GI::alloc_op::Delete,
            ptr, _ReturnAddress()
        );
    }
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    GI::EnableGuard guard;
    if (guard) {
        GI::global.onDeallocate(
            GI::alloc_op::Delete_Array,
            ptr, _ReturnAddress()
        );
    }
}