#ifndef OPERATOR_NEW_HOOKER_H
#define OPERATOR_NEW_HOOKER_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
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

    struct GlobalData {
        GlobalData();
        ~GlobalData();
        // ���ڼ�¼new/delete�ĵ���
        void onAllocate(alloc_op, void*, size_t, void*);
        void onDeallocate(alloc_op, void*, void*);
    };

    // ���AllocInfo
    bool add_AllcInfo(alloc_op, void*, size_t, void*);
    // ����hook
    void EnableHook();
    // ����hook
    void DisableHook();
    // ����ڴ�й©
    void checkLeaks();
    // ���allocs��ƥ��
    bool checkAllocsMatch(alloc_op, alloc_op);

    // ��ʼ������
    void InitializeDbgHelp();
    // ����ת��������ַΪ����λ��
    std::string getCallerInfo(void*);

    // ���ڳ�ʼ��ȫ�ֱ���
    extern GlobalData globalData;
    // ���ڼ�¼new/delete�ĵ���
    extern std::map<void*, AllocInfo> allocs;
    // ���ڿ����Ƿ�����hook
    extern bool enable;
}

#undef OPERATOR_NEW_HOOKER_H
#endif