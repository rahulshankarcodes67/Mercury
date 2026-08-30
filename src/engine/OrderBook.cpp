#include "OrderBook.h"
#include <algorithm>
#include <chrono>

namespace mercury {

std::vector<Trade> OrderBook::addOrder(Order* order) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // First, try to match the order
    std::vector<Trade> trades = matchOrder(order);

    // If order is not fully filled, add it to the book
    if (order->filled_quantity < order->quantity) {
        if (order->side == Side::Buy) {
            bids_[order->price].push_back(order);
        } else {
            asks_[order->price].push_back(order);
        }
    }

    return trades;
}

std::vector<Trade> OrderBook::matchOrder(Order* order) {
    std::vector<Trade> trades;
    uint32_t remaining_quantity = order->quantity - order->filled_quantity;

    if (order->side == Side::Buy) {
        auto it = asks_.begin();
        while (it != asks_.end() && remaining_quantity > 0 && it->first <= order->price) {
            auto& order_list = it->second;
            auto list_it = order_list.begin();
            
            while (list_it != order_list.end() && remaining_quantity > 0) {
                Order* maker = *list_it;
                uint32_t maker_remaining = maker->quantity - maker->filled_quantity;
                uint32_t trade_quantity = std::min(remaining_quantity, maker_remaining);

                // Execute trade
                Trade trade;
                trade.symbol_id = symbol_id_;
                trade.price = maker->price; // Price improvement for taker
                trade.quantity = trade_quantity;
                trade.buyer_order_id = order->id;
                trade.seller_order_id = maker->id;
                trade.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                
                trades.push_back(trade);

                maker->filled_quantity += trade_quantity;
                order->filled_quantity += trade_quantity;
                remaining_quantity -= trade_quantity;

                if (maker->filled_quantity == maker->quantity) {
                    list_it = order_list.erase(list_it); // Remove fully filled maker order
                } else {
                    ++list_it;
                }
            }

            if (order_list.empty()) {
                it = asks_.erase(it);
            } else {
                ++it;
            }
        }
    } else { // Sell order
        auto it = bids_.begin();
        while (it != bids_.end() && remaining_quantity > 0 && it->first >= order->price) {
            auto& order_list = it->second;
            auto list_it = order_list.begin();
            
            while (list_it != order_list.end() && remaining_quantity > 0) {
                Order* maker = *list_it;
                uint32_t maker_remaining = maker->quantity - maker->filled_quantity;
                uint32_t trade_quantity = std::min(remaining_quantity, maker_remaining);

                Trade trade;
                trade.symbol_id = symbol_id_;
                trade.price = maker->price; 
                trade.quantity = trade_quantity;
                trade.buyer_order_id = maker->id;
                trade.seller_order_id = order->id;
                trade.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
                
                trades.push_back(trade);

                maker->filled_quantity += trade_quantity;
                order->filled_quantity += trade_quantity;
                remaining_quantity -= trade_quantity;

                if (maker->filled_quantity == maker->quantity) {
                    list_it = order_list.erase(list_it);
                } else {
                    ++list_it;
                }
            }

            if (order_list.empty()) {
                it = bids_.erase(it);
            } else {
                ++it;
            }
        }
    }

    return trades;
}

void OrderBook::cancelOrder(Order* order) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (order->side == Side::Buy) {
        auto it = bids_.find(order->price);
        if (it != bids_.end()) {
            it->second.remove(order);
            if (it->second.empty()) {
                bids_.erase(it);
            }
        }
    } else {
        auto it = asks_.find(order->price);
        if (it != asks_.end()) {
            it->second.remove(order);
            if (it->second.empty()) {
                asks_.erase(it);
            }
        }
    }
}

} // namespace mercury
