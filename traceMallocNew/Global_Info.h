#ifndef OPERATOR_NEW_HOOKER_H
#define OPERATOR_NEW_HOOKER_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <intrin.h> // 包含_ReturnAddress
#include <map>
#include <new> // 包含标准的new头文件
#include <string> 
#include <Windows.h>
#include <DbgHelp.h> // DbgHelp头文件必须在Windows.h之后

#pragma comment(lib, "DbgHelp.lib") // 链接DbgHelp.lib

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
        alloc_op op; // 操作类型
        void* ptr; // 分配的内存地址
        size_t size; // 分配的内存大小
        void* caller; // 调用new的位置
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
        // 用于记录new/delete的调用
        void onAllocate(alloc_op, void*, size_t, void*);
        void onDeallocate(alloc_op, void*, void*);
    };

    // 添加AllocInfo
    bool add_AllcInfo(alloc_op, void*, size_t, void*);
    // 启用hook
    void EnableHook();
    // 禁用hook
    void DisableHook();
    // 检查内存泄漏
    void checkLeaks();
    // 检查allocs是匹配
    bool checkAllocsMatch(alloc_op, alloc_op);

    // 初始化函数
    void InitializeDbgHelp();
    // 用于转化函数地址为代码位置
    std::string getCallerInfo(void*);

    // 用于初始化全局变量
    extern GlobalData globalData;
    // 用于记录new/delete的调用
    extern std::map<void*, AllocInfo> allocs;
    // 用于控制是否启用hook
    extern bool enable;
}

#undef OPERATOR_NEW_HOOKER_H
#endif