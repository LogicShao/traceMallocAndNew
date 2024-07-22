#ifndef OPERATOR_NEW_HOOKER_CPP
#define OPERATOR_NEW_HOOKER_CPP
#endif

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <intrin.h> // 包含_ReturnAddress
#include <map>
#include <new> // 包含标准的new头文件
#include <string> 

namespace hooker {
    struct AllocInfo {
        void* ptr; // 分配的内存地址
		size_t size; // 分配的内存大小
		void* caller; // 调用new的位置
	};

    struct GlobalData {
        GlobalData();
        ~GlobalData();
    };

    // 用于初始化全局变量
    extern GlobalData globalData;

    // 用于记录new/delete的调用
    extern std::map<void*, AllocInfo> allocs;
    // 用于控制是否启用hook
    extern bool enable;
    // 添加AllocInfo
    void add_AllcInfo(void* ptr, size_t size, void* caller);
    // 启用hook
    void EnableHook();
    // 禁用hook
    void DisableHook();
    // 检查内存泄漏
    void checkLeaks();

    // 初始化函数
    void InitializeDbgHelp();
    // 用于转化函数地址为代码位置
    std::string getCallerInfo(void* caller);
}

// 使用inline避免链接时的多重定义问题
inline void* operator new(size_t size); // hook new
inline void operator delete(void* ptr) noexcept; // hook delete

#undef OPERATOR_NEW_HOOKER_CPP
