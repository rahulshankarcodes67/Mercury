#pragma once
#include "OrderBook.h"
#include "../ds/OrderTracker.h"
#include "../ds/MemoryPool.h"
#include <unordered_map>
#include <mutex>
#include <memory>

namespace mercury {

class MatchingEngine {
public:
    explicit MatchingEngine(size_t max_orders) 
        : order_pool_(max_orders) {}

    // Processes a new incoming order, returning executed trades
    std::vector<Trade> processOrder(uint64_t id, uint32_t symbol_id, Side side, uint32_t price, uint32_t quantity);
    
    // Cancels an existing order
    bool cancelOrder(uint64_t id);

private:
    std::unordered_map<uint32_t, std::unique_ptr<OrderBook>> order_books_;
    std::mutex mutex_;
    
    OrderTracker tracker_;
    MemoryPool<Order> order_pool_;

    OrderBook* getOrCreateOrderBook(uint32_t symbol_id);
};

} // namespace mercury
