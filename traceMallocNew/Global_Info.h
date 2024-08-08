#ifndef OPERATOR_NEW_HOOKER_H
#define OPERATOR_NEW_HOOKER_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <chrono>
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

    class GlobalData { // 单例模式
    public:
        GlobalData(GlobalData &) = delete; // 禁用拷贝构造函数
        GlobalData& operator=(GlobalData const&) = delete; // 禁用赋值构造函数

        static GlobalData& getInstance(); // 获取实例
        bool getEnable() const; // 获取hook状态
    
        void EnableHook(); // 启用hook
        void DisableHook(); // 禁用hook

        // 用于记录new/delete的调用
        bool add_AllcInfo(alloc_op, void*, size_t, void*);
        void onAllocate(alloc_op, void*, size_t, void*); // 分配内存时调用
        void onDeallocate(alloc_op, void*, void*); // 释放内存时调用

    private:
        std::map<void*, AllocInfo> allocs; // 用于记录分配的内存
        bool enable = false; // 是否启用hook

        GlobalData(); // 防止外部调用构造函数
        ~GlobalData(); // 防止外部调用析构函数

        // 检查内存泄漏
        void checkLeaks();
    };

    // 获取全局数据实例
    GlobalData& getGD();

    // 检查allocs是匹配
    bool checkAllocsMatch(alloc_op, alloc_op);

    // 初始化DbgHelp用于获取CallerInfo
    void InitializeDbgHelp();
    // 用于转化函数地址为代码位置
    std::string getCallerInfo(void*);
}

#undef OPERATOR_NEW_HOOKER_H
#endif