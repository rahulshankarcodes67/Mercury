#pragma once
#include "Order.h"
#include <unordered_map>
#include <mutex>

namespace mercury {

class OrderTracker {
public:
    OrderTracker() = default;

    void addOrder(Order* order) {
        std::lock_guard<std::mutex> lock(mutex_);
        orders_[order->id] = order;
    }

    Order* getOrder(uint64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = orders_.find(id);
        if (it != orders_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void removeOrder(uint64_t id) {
        std::lock_guard<std::mutex> lock(mutex_);
        orders_.erase(id);
    }

private:
    // In a real HFT engine, we'd use a lock-free map or a custom flat array if IDs are sequential.
    // std::unordered_map provides O(1) lookup on average.
    std::unordered_map<uint64_t, Order*> orders_;
    std::mutex mutex_;
};

} // namespace mercury
