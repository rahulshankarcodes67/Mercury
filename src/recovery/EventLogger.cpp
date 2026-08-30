#include "EventLogger.h"

namespace mercury {

EventLogger::EventLogger(const std::string& filename) {
    file_.open(filename, std::ios::out | std::ios::app | std::ios::binary);
}

EventLogger::~EventLogger() {
    if (file_.is_open()) {
        file_.close();
    }
}

void EventLogger::logNewOrder(const Order* order) {
    std::lock_guard<std::mutex> lock(mutex_);
    char type = 'N';
    file_.write(&type, 1);
    file_.write(reinterpret_cast<const char*>(order), sizeof(Order));
    // In a real HFT system we wouldn't flush every time (we'd use a background thread or batching), 
    // but we do it here for simplicity/correctness.
    file_.flush();
}

void EventLogger::logCancelOrder(uint64_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    char type = 'C';
    file_.write(&type, 1);
    file_.write(reinterpret_cast<const char*>(&id), sizeof(uint64_t));
    file_.flush();
}

void EventLogger::logTrade(const Trade& trade) {
    std::lock_guard<std::mutex> lock(mutex_);
    char type = 'T';
    file_.write(&type, 1);
    file_.write(reinterpret_cast<const char*>(&trade), sizeof(Trade));
    file_.flush();
}

} // namespace mercury
