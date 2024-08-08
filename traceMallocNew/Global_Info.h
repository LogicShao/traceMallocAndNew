#ifndef OPERATOR_NEW_HOOKER_H
#define OPERATOR_NEW_HOOKER_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <intrin.h> // ����_ReturnAddress
#include <map>
#include <new> // ������׼��newͷ�ļ�
#include <string> 
#include <Windows.h>
#include <DbgHelp.h> // DbgHelpͷ�ļ�������Windows.h֮��

#pragma comment(lib, "DbgHelp.lib") // ����DbgHelp.lib

namespace Global_Info {
    constexpr const char* alloc_op_str[] = {
        "New",
        "Delete",
        "New Array",
        "Delete Array"
    };

    constexpr bool AllocOpIsAllocation[] = {
        true,
        false,
        true,
        false
    };

    enum class alloc_op {
        New,
        Delete,
        New_Array,
        Delete_Array
        /**
         * TO DO: Malloc,
         * TO DO: Free
         */
    };

    struct AllocInfo {
        alloc_op op; // ��������
        void* ptr; // ������ڴ��ַ
        size_t size; // ������ڴ��С
        void* caller; // ����new��λ��
    };

    struct EnableGuard {
        bool was_enable;
        EnableGuard();
        ~EnableGuard();
        explicit operator bool() const;
    };

    class GlobalData { // ����ģʽ
    public:
        GlobalData(GlobalData &) = delete; // ���ÿ������캯��
        GlobalData& operator=(GlobalData const&) = delete; // ���ø�ֵ���캯��

        static GlobalData& getInstance(); // ��ȡʵ��
        bool getEnable() const; // ��ȡhook״̬
    
        void EnableHook(); // ����hook
        void DisableHook(); // ����hook

        // ���ڼ�¼new/delete�ĵ���
        bool add_AllcInfo(alloc_op, void*, size_t, void*);
        void onAllocate(alloc_op, void*, size_t, void*); // �����ڴ�ʱ����
        void onDeallocate(alloc_op, void*, void*); // �ͷ��ڴ�ʱ����

    private:
        std::map<void*, AllocInfo> allocs; // ���ڼ�¼������ڴ�
        bool enable = false; // �Ƿ�����hook

        GlobalData(); // ��ֹ�ⲿ���ù��캯��
        ~GlobalData(); // ��ֹ�ⲿ������������

        // ����ڴ�й©
        void checkLeaks();
    };

    // ��ȡȫ������ʵ��
    GlobalData& getGD();

    // ���allocs��ƥ��
    bool checkAllocsMatch(alloc_op, alloc_op);

    // ��ʼ��DbgHelp���ڻ�ȡCallerInfo
    void InitializeDbgHelp();
    // ����ת��������ַΪ����λ��
    std::string getCallerInfo(void*);
}

#undef OPERATOR_NEW_HOOKER_H
#endif