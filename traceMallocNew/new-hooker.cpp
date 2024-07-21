#include "new-hooker.h"

namespace hooker {
	std::map<void*, size_t> allocs;
	bool enable = false;

    void EnableHook() {
		enable = true;
	}

    void DisableHook() {
		enable = false;
	}
}

// 使用inline避免链接时的多重定义问题
inline void* operator new(size_t size) {
    void* ptr = malloc(size); // 分配内存

    // 如果hooker::enable为true，则记录分配的内存
    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        if (ptr != nullptr) {
            printf("new: size=%zu at %p\n", size, ptr);
            hooker::allocs[ptr] = size;
        }
    }

    // 恢复hooker::enable的值
    hooker::enable = was_enable;

    // 处理分配失败的情况
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    // 标准要求delete nullptr是安全的
    if (ptr == nullptr) return;

    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        printf("delete: %p\n", ptr);
        hooker::allocs.erase(ptr);
    }

    free(ptr);
    hooker::enable = was_enable;
}