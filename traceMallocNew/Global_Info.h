#ifndef OPERATOR_NEW_HOOKER_H
#define OPERATOR_NEW_HOOKER_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <intrin.h> // ����_ReturnAddress
#include <map>
#include <iostream>
#include <queue>
#include <new> // ������׼��newͷ�ļ�
#include <string> 
#include <Windows.h>
#include <DbgHelp.h> // DbgHelpͷ�ļ�������Windows.h֮��
#include "nanosvg.h" // ��������SVG

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

    struct Lifeline {
		void* ptr; // ������ڴ��ַ
		time_t start; // �����ʱ��
		time_t end; // �ͷŵ�ʱ��
    };

    struct GlobalData {
        std::map<void*, AllocInfo> allocs; // ���ڼ�¼������ڴ�
		std::vector<Lifeline> lifelines; // ���ڼ�¼�����ʱ��
        bool enable = false; // �Ƿ�����hook

        GlobalData();
        ~GlobalData();

        bool getEnable() const; // ��ȡhook״̬
        void EnableHook(); // ����hook
        void DisableHook(); // ����hook

        bool add_AllcInfo(alloc_op, void*, size_t, void*); // ���ڼ�¼new/delete�ĵ���
        void add_Action(alloc_op, void*, size_t, void*); // ���ڼ�¼�����ʱ��

        void onAllocate(alloc_op, void*, size_t, void*); // �����ڴ�ʱ����
        void onDeallocate(alloc_op, void*, void*); // �ͷ��ڴ�ʱ����

        void checkLeaks(); // ����ڴ�й©
        void outActions(); // ���������ڴ�

        void generateSVG(); // ����SVG
    };

    extern GlobalData global; // ȫ�ֱ���

    // ���allocs��ƥ��
    bool checkAllocsMatch(alloc_op, alloc_op);

    // ��ʼ��DbgHelp���ڻ�ȡCallerInfo
    void InitializeDbgHelp();
    // ����ת��������ַΪ����λ��
    std::string getCallerInfo(void*);
}

namespace GI = Global_Info;

#undef OPERATOR_NEW_HOOKER_H
#endif