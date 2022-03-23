#include "prometheus/gauge.h"

namespace prometheus {

Gauge::Gauge(const double value) : value_{value} {}

void Gauge::Increment() { Increment(1.0); }

void Gauge::Increment(const double value) { Change(value); }

void Gauge::Decrement() { Decrement(1.0); }

void Gauge::Decrement(const double value) { Change(-1.0 * value); }

void Gauge::Set(const double value) {
  value_.store(value);
  time_.store(std::time(nullptr));
}

void Gauge::Change(const double value) {
  // C++ 20 will add std::atomic::fetch_add support for floating point types
  auto current = value_.load();
  while (!value_.compare_exchange_weak(current, current + value)) {
    // intentionally empty block
  }
  time_.store(std::time(nullptr));
}

void Gauge::SetToCurrentTime() {
  const auto time = std::time(nullptr);
  Set(static_cast<double>(time));
}

double Gauge::Value() const { return value_; }

ClientMetric Gauge::Collect() const {
  ClientMetric metric;
  metric.gauge.value = Value();
  return metric;
}

bool Gauge::Expired(std::time_t time, double seconds) const {
  return std::difftime(time, time_) > seconds;
}

}  // namespace prometheus
