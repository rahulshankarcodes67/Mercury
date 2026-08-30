#include "MatchingEngine.h"

namespace mercury {

OrderBook* MatchingEngine::getOrCreateOrderBook(uint32_t symbol_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = order_books_.find(symbol_id);
    if (it == order_books_.end()) {
        auto book = std::make_unique<OrderBook>(symbol_id);
        OrderBook* raw_ptr = book.get();
        order_books_[symbol_id] = std::move(book);
        return raw_ptr;
    }
    return it->second.get();
}

std::vector<Trade> MatchingEngine::processOrder(uint64_t id, uint32_t symbol_id, Side side, uint32_t price, uint32_t quantity) {
    try {
        Order* order = order_pool_.allocate(id, symbol_id, side, price, quantity);
        tracker_.addOrder(order);
        
        OrderBook* book = getOrCreateOrderBook(symbol_id);
        std::vector<Trade> trades = book->addOrder(order);
        
        if (order->filled_quantity == order->quantity) {
            tracker_.removeOrder(order->id);
            order_pool_.deallocate(order);
        }

        return trades;
    } catch (const std::bad_alloc&) {
        // Pool exhausted
        return {};
    }
}

bool MatchingEngine::cancelOrder(uint64_t id) {
    Order* order = tracker_.getOrder(id);
    if (!order) return false;

    OrderBook* book = getOrCreateOrderBook(order->symbol_id);
    book->cancelOrder(order);
    
    tracker_.removeOrder(id);
    order_pool_.deallocate(order);
    return true;
}

} // namespace mercury
