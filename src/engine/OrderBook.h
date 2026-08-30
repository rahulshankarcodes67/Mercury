#pragma once
#include "Order.h"
#include <map>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

namespace mercury {

struct Trade {
    uint32_t symbol_id;
    uint32_t price;
    uint32_t quantity;
    uint64_t buyer_order_id;
    uint64_t seller_order_id;
    uint64_t timestamp;
};

class OrderBook {
public:
    explicit OrderBook(uint32_t symbol_id) : symbol_id_(symbol_id) {}

    std::vector<Trade> addOrder(Order* order);
    void cancelOrder(Order* order);

private:
    uint32_t symbol_id_;
    std::mutex mutex_;

    // Bids sorted descending (highest price first)
    std::map<uint32_t, std::list<Order*>, std::greater<uint32_t>> bids_; 
    
    // Asks sorted ascending (lowest price first)
    std::map<uint32_t, std::list<Order*>> asks_;                         
    
    std::vector<Trade> matchOrder(Order* order);
};

} // namespace mercury
