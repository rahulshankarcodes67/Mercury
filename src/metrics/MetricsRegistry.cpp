#include "MetricsRegistry.h"

namespace mercury {

MetricsRegistry::MetricsRegistry() {
#ifdef USE_PROMETHEUS
    registry_ = std::make_shared<prometheus::Registry>();
    
    // Start Exposer on port 8080
    exposer_ = std::make_unique<prometheus::Exposer>("0.0.0.0:8080");
    exposer_->RegisterCollectable(registry_);

    counter_family_ = &prometheus::BuildCounter()
                            .Name("mercury_events_total")
                            .Help("Total number of events processed")
                            .Register(*registry_);
                            
    orders_processed_ = &counter_family_->Add({{"type", "order_processed"}});
    trades_executed_ = &counter_family_->Add({{"type", "trade_executed"}});

    histogram_family_ = &prometheus::BuildHistogram()
                            .Name("mercury_order_latency_nanoseconds")
                            .Help("Latency of order processing in nanoseconds")
                            .Register(*registry_);
                            
    // Buckets for latency (e.g., 100ns to 1ms)
    latency_histogram_ = &histogram_family_->Add({}, prometheus::Histogram::BucketBoundaries{
        100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000
    });
#endif
}

MetricsRegistry::~MetricsRegistry() = default;

void MetricsRegistry::recordOrderProcessed() {
#ifdef USE_PROMETHEUS
    orders_processed_->Increment();
#endif
}

void MetricsRegistry::recordTradeExecuted() {
#ifdef USE_PROMETHEUS
    trades_executed_->Increment();
#endif
}

void MetricsRegistry::recordLatency(std::chrono::nanoseconds latency) {
#ifdef USE_PROMETHEUS
    latency_histogram_->Observe(static_cast<double>(latency.count()));
#endif
}

} // namespace mercury
