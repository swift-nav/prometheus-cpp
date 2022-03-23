#include "prometheus/counter.h"

namespace prometheus {

void Counter::Increment() { gauge_.Increment(); }

void Counter::Increment(const double val) {
  if (val < 0.0) {
    return;
  }
  gauge_.Increment(val);
}

double Counter::Value() const { return gauge_.Value(); }

ClientMetric Counter::Collect() const {
  ClientMetric metric;
  metric.counter.value = Value();
  return metric;
}

bool Counter::Expired(const std::chrono::steady_clock::time_point& time,
                      const std::chrono::seconds& ttl) {
  (void)time;
  (void)ttl;
  return false;
}

}  // namespace prometheus
