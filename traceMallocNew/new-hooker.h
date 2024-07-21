#ifndef OPERATOR_NEW_HOOKER_CPP
#define OPERATOR_NEW_HOOKER_CPP
#endif

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <new> // ������׼��newͷ�ļ�
#include <map>

namespace hooker {
    // ���ڼ�¼new/delete�ĵ���
    extern std::map<void*, size_t> allocs;
    // ���ڿ����Ƿ�����hook
    extern bool enable;

    void EnableHook(); // ����hook
    void DisableHook(); // ����hook
}

// ʹ��inline��������ʱ�Ķ��ض�������
inline void* operator new(size_t size); // hook new
inline void operator delete(void* ptr) noexcept; // hook delete

#undef OPERATOR_NEW_HOOKER_CPP
