#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <new>

namespace mercury {

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t pool_size) : capacity(pool_size) {
        // Allocate raw memory so we don't call default constructors unnecessarily
        pool = static_cast<T*>(::operator new(capacity * sizeof(T)));
        free_indices.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            // Push in reverse order so we allocate from the beginning
            free_indices.push_back(capacity - 1 - i); 
        }
    }

    ~MemoryPool() {
        ::operator delete(pool);
    }

    template <typename... Args>
    T* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (free_indices.empty()) {
            throw std::bad_alloc();
        }
        size_t index = free_indices.back();
        free_indices.pop_back();
        T* ptr = &pool[index];
        new (ptr) T(std::forward<Args>(args)...); // Placement new
        return ptr;
    }

    void deallocate(T* ptr) {
        if (!ptr) return;
        ptr->~T(); // Call destructor
        
        // Calculate index
        size_t index = ptr - pool;
        
        std::lock_guard<std::mutex> lock(mutex_);
        free_indices.push_back(index);
    }

private:
    T* pool;
    std::vector<size_t> free_indices;
    size_t capacity;
    std::mutex mutex_;
};

} // namespace mercury
