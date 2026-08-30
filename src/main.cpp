#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>

#include "engine/MatchingEngine.h"
#include "recovery/EventLogger.h"
#include "recovery/SnapshotManager.h"
#include "metrics/MetricsRegistry.h"

using namespace mercury;

int main() {
    std::cout << "Starting Mercury Trading Engine..." << std::endl;

    // Initialize Core Components
    MatchingEngine engine(2000000); // Pool for 2M orders
    EventLogger logger("events.log");
    SnapshotManager snapshot_mgr("./snapshots");
    MetricsRegistry metrics;

    std::cout << "Components Initialized. Simulating Order Flow..." << std::endl;

    uint32_t symbol_id = 1; // AAPL

    std::mt19937 gen(1337);
    std::uniform_int_distribution<> price_dist(14900, 15100); // $149.00 to $151.00
    std::uniform_int_distribution<> qty_dist(10, 1000);
    std::uniform_int_distribution<> side_dist(0, 1);

    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t total_orders = 1'000'000;

    for (uint64_t i = 1; i <= total_orders; ++i) {
        Side side = side_dist(gen) == 0 ? Side::Buy : Side::Sell;
        uint32_t price = price_dist(gen);
        uint32_t qty = qty_dist(gen);

        auto t1 = std::chrono::high_resolution_clock::now();
        
        std::vector<Trade> trades = engine.processOrder(i, symbol_id, side, price, qty);
        
        auto t2 = std::chrono::high_resolution_clock::now();
        metrics.recordLatency(std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1));
        metrics.recordOrderProcessed();

        for (const auto& trade : trades) {
            metrics.recordTradeExecuted();
            logger.logTrade(trade);
        }

        if (i % 100000 == 0) {
            snapshot_mgr.takeSnapshot(engine, i);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;
    
    std::cout << "Processed " << total_orders << " orders in " << diff.count() << " seconds." << std::endl;
    std::cout << "Throughput: " << (total_orders / diff.count()) << " orders/sec." << std::endl;

#ifdef USE_PROMETHEUS
    std::cout << "Prometheus metrics available on http://localhost:8080/metrics" << std::endl;
    std::cout << "Sleeping to allow metrics scraping. Press Ctrl+C to exit." << std::endl;
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
#endif

    return 0;
}
