#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include "../engine/OrderBook.h" 
#include "../engine/Order.h"

namespace mercury {

enum class EventType {
    NewOrder,
    CancelOrder,
    TradeExecution
};

class EventLogger {
public:
    explicit EventLogger(const std::string& filename);
    ~EventLogger();

    void logNewOrder(const Order* order);
    void logCancelOrder(uint64_t id);
    void logTrade(const Trade& trade);

private:
    std::ofstream file_;
    std::mutex mutex_;
};

} // namespace mercury
