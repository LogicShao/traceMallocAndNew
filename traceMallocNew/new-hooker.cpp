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

// ʹ��inline��������ʱ�Ķ��ض�������
inline void* operator new(size_t size) {
    void* ptr = malloc(size); // �����ڴ�

    // ���hooker::enableΪtrue�����¼������ڴ�
    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        if (ptr != nullptr) {
            printf("new: size=%zu at %p\n", size, ptr);
            hooker::allocs[ptr] = size;
        }
    }

    // �ָ�hooker::enable��ֵ
    hooker::enable = was_enable;

    // �������ʧ�ܵ����
    if (ptr == nullptr) {
        throw std::bad_alloc();
    }

    return ptr;
}

inline void operator delete(void* ptr) noexcept {
    // ��׼Ҫ��delete nullptr�ǰ�ȫ��
    if (ptr == nullptr) return;

    bool was_enable = std::exchange(hooker::enable, false);
    if (was_enable) {
        printf("delete: %p\n", ptr);
        hooker::allocs.erase(ptr);
    }

    free(ptr);
    hooker::enable = was_enable;
}