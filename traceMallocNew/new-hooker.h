#ifndef OPERATOR_NEW_HOOKER_CPP
#define OPERATOR_NEW_HOOKER_CPP
#endif

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <intrin.h> // ����_ReturnAddress
#include <map>
#include <new> // ������׼��newͷ�ļ�
#include <string> 

namespace hooker {
    struct AllocInfo {
        void* ptr; // ������ڴ��ַ
		size_t size; // ������ڴ��С
		void* caller; // ����new��λ��
	};

    struct GlobalData {
        GlobalData();
        ~GlobalData();
    };

    // ���ڳ�ʼ��ȫ�ֱ���
    extern GlobalData globalData;

    // ���ڼ�¼new/delete�ĵ���
    extern std::map<void*, AllocInfo> allocs;
    // ���ڿ����Ƿ�����hook
    extern bool enable;
    // ���AllocInfo
    void add_AllcInfo(void* ptr, size_t size, void* caller);
    // ����hook
    void EnableHook();
    // ����hook
    void DisableHook();
    // ����ڴ�й©
    void checkLeaks();

    // ��ʼ������
    void InitializeDbgHelp();
    // ����ת��������ַΪ����λ��
    std::string getCallerInfo(void* caller);
}

// ʹ��inline��������ʱ�Ķ��ض�������
inline void* operator new(size_t size); // hook new
inline void operator delete(void* ptr) noexcept; // hook delete

#undef OPERATOR_NEW_HOOKER_CPP
