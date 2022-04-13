#pragma once

#include <atomic>
#include <chrono>

#include "prometheus/client_metric.h"
#include "prometheus/detail/builder.h"  // IWYU pragma: export
#include "prometheus/detail/core_export.h"
#include "prometheus/metric_type.h"

namespace prometheus {

/// \brief A gauge metric to represent a value that can arbitrarily go up and
/// down.
///
/// The class represents the metric type gauge:
/// https://prometheus.io/docs/concepts/metric_types/#gauge
///
/// Gauges are typically used for measured values like temperatures or current
/// memory usage, but also "counts" that can go up and down, like the number of
/// running processes.
///
/// The class is thread-safe. No concurrent call to any API of this type causes
/// a data race.
class PROMETHEUS_CPP_CORE_EXPORT Gauge {
 public:
  static const MetricType metric_type{MetricType::Gauge};

  /// \brief Create a gauge that starts at 0.
  Gauge() = default;

  /// \brief Create a gauge that starts at the given amount.
  Gauge(double);

  /// \brief Increment the gauge by 1.
  void Increment();

  /// \brief Increment the gauge by the given amount.
  void Increment(double);

  /// \brief Decrement the gauge by 1.
  void Decrement();

  /// \brief Decrement the gauge by the given amount.
  void Decrement(double);

  /// \brief Set the gauge to the given value.
  void Set(double);

  /// \brief Set the gauge to the current unixtime in seconds.
  void SetToCurrentTime();

  /// \brief Get the current value of the gauge.
  double Value() const;

  /// \brief Get the current value of the gauge.
  ///
  /// Collect is called by the Registry when collecting metrics.
  ClientMetric Collect() const;

  /// \brief Check if the gauge has expired.
  ///
  /// Expires is called by the Registry when collecting metrics.
  ///
  /// A gauge has expired if it has been more than `ttl` seconds
  /// since `time`.
  ///
  /// \param time The current time.
  /// \param time Time to live in seconds.
  ///
  /// \return Has the gauge expired.
  bool Expired(const std::chrono::system_clock::time_point& time,
               const std::chrono::seconds& ttl) const;

 private:
  void Change(double);
  std::atomic<double> value_{0.0};
  std::atomic<std::chrono::system_clock::duration> time_{
      std::chrono::system_clock::now().time_since_epoch()};
};

/// \brief Return a builder to configure and register a Gauge metric.
///
/// @copydetails Family<>::Family()
///
/// Example usage:
///
/// \code
/// auto registry = std::make_shared<Registry>();
/// auto& gauge_family = prometheus::BuildGauge()
///                          .Name("some_name")
///                          .Help("Additional description.")
///                          .Labels({{"key", "value"}})
///                          .TTL(360)
///                          .Register(*registry);
///
/// ...
/// \endcode
///
/// \return An object of unspecified type T, i.e., an implementation detail
/// except that it has the following members:
///
/// - Name(const std::string&) to set the metric name,
/// - Help(const std::string&) to set an additional description.
/// - Labels(const Labels&) to assign a set of
///   key-value pairs (= labels) to the metric.
/// - TTL(const std::chrono::seconds&)` to set the time to live. Gauges that
///   are not updated within `ttl` seconds will not be collected.
///
/// To finish the configuration of the Gauge metric register it with
/// Register(Registry&).
PROMETHEUS_CPP_CORE_EXPORT detail::Builder<Gauge> BuildGauge();

}  // namespace prometheus
