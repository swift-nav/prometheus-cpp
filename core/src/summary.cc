#include "prometheus/summary.h"

#include <utility>

namespace prometheus {

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::milliseconds max_age, const int age_buckets)
    : Summary(quantiles, std::chrono::system_clock::now(), max_age,
              age_buckets) {}

Summary::Summary(const Quantiles& quantiles,
                 const std::chrono::system_clock::time_point& creation_time,
                 const std::chrono::milliseconds max_age, const int age_buckets)
    : quantiles_{quantiles},
      count_{0},
      sum_{0},
      quantile_values_{quantiles_, creation_time, max_age, age_buckets} {}

void Summary::Observe(const double value) {
  Observe(value, std::chrono::system_clock::now());
}

void Summary::Observe(const double value,
                      const std::chrono::system_clock::time_point& time) {
  std::lock_guard<std::mutex> lock(mutex_);

  count_ += 1;
  sum_ += value;
  quantile_values_.insert(value, time);
}

ClientMetric Summary::Collect() const {
  return Collect(std::chrono::system_clock::now());
}

ClientMetric Summary::Collect(
    const std::chrono::system_clock::time_point& time) const {
  auto metric = ClientMetric{};

  std::lock_guard<std::mutex> lock(mutex_);

  metric.summary.quantile.reserve(quantiles_.size());
  for (const auto& quantile : quantiles_) {
    auto metricQuantile = ClientMetric::Quantile{};
    metricQuantile.quantile = quantile.quantile;
    metricQuantile.value = quantile_values_.get(quantile.quantile, time);
    metric.summary.quantile.push_back(std::move(metricQuantile));
  }
  metric.summary.sample_count = count_;
  metric.summary.sample_sum = sum_;

  return metric;
}

bool Summary::Expired(const std::chrono::system_clock::time_point& time,
                      const std::chrono::seconds& ttl) {
  (void)time;
  (void)ttl;
  return false;
}

}  // namespace prometheus
