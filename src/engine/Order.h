#pragma once
#include <cstdint>
#include <chrono>
#include <string>

namespace mercury {

enum class Side {
    Buy,
    Sell
};

struct Order {
    uint64_t id;
    uint32_t symbol_id;
    Side side;
    uint32_t price;
    uint32_t quantity;
    uint32_t filled_quantity;
    uint64_t timestamp;

    Order(uint64_t id, uint32_t symbol_id, Side side, uint32_t price, uint32_t quantity)
        : id(id), symbol_id(symbol_id), side(side), price(price), 
          quantity(quantity), filled_quantity(0) {
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    // Default constructor needed for some containers/pools
    Order() : id(0), symbol_id(0), side(Side::Buy), price(0), quantity(0), filled_quantity(0), timestamp(0) {}
};

} // namespace mercury
