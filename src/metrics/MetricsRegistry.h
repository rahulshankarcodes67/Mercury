#pragma once
#include <memory>
#include <chrono>

#ifdef USE_PROMETHEUS
#include <prometheus/registry.h>
#include <prometheus/exposer.h>
#include <prometheus/counter.h>
#include <prometheus/histogram.h>
#endif

namespace mercury {

class MetricsRegistry {
public:
    MetricsRegistry();
    ~MetricsRegistry();

    void recordOrderProcessed();
    void recordTradeExecuted();
    void recordLatency(std::chrono::nanoseconds latency);

private:
#ifdef USE_PROMETHEUS
    std::shared_ptr<prometheus::Registry> registry_;
    std::unique_ptr<prometheus::Exposer> exposer_;
    
    prometheus::Family<prometheus::Counter>* counter_family_;
    prometheus::Counter* orders_processed_;
    prometheus::Counter* trades_executed_;

    prometheus::Family<prometheus::Histogram>* histogram_family_;
    prometheus::Histogram* latency_histogram_;
#endif
};

} // namespace mercury
