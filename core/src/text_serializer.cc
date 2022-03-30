#include "prometheus/text_serializer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <locale>
#include <ostream>
#include <string>

#include "prometheus/client_metric.h"
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

namespace prometheus {

namespace {

// Write a double as a string, with proper formatting for infinity and NaN
void WriteValue(std::ostream& out, double value) {
  if (std::isnan(value)) {
    out << "Nan";
  } else if (std::isinf(value)) {
    out << (value < 0 ? "-Inf" : "+Inf");
  } else {
    out << value;
  }
}

void WriteValue(std::ostream& out, const std::string& value) {
  for (auto c : value) {
    switch (c) {
      case '\n':
        out << '\\' << 'n';
        break;

      case '\\':
        out << '\\' << c;
        break;

      case '"':
        out << '\\' << c;
        break;

      default:
        out << c;
        break;
    }
  }
}

// Write a line header: metric name and labels
template <typename T = std::string>
void WriteHead(std::ostream& out, const MetricFamily& family,
               const ClientMetric& metric, const std::string& suffix = "",
               const std::string& extraLabelName = "",
               const T& extraLabelValue = T()) {
  out << family.name << suffix;
  if (!metric.label.empty() || !extraLabelName.empty()) {
    out << "{";
    const char* prefix = "";

    for (auto& lp : metric.label) {
      out << prefix << lp.name << "=\"";
      WriteValue(out, lp.value);
      out << "\"";
      prefix = ",";
    }
    if (!extraLabelName.empty()) {
      out << prefix << extraLabelName << "=\"";
      WriteValue(out, extraLabelValue);
      out << "\"";
    }
    out << "}";
  }
  out << " ";
}

// Write a line trailer: timestamp
void WriteTail(std::ostream& out, const ClientMetric& metric) {
  if (metric.timestamp != std::chrono::seconds::zero()) {
    using FloatSeconds = std::chrono::duration<double>;
    out << " "
        << std::chrono::duration_cast<FloatSeconds>(metric.timestamp).count();
  }
  out << "\n";
}

void SerializeCounter(std::ostream& out, const MetricFamily& family,
                      const ClientMetric& metric) {
  WriteHead(out, family, metric, "_total");
  WriteValue(out, metric.counter.value);
  WriteTail(out, metric);
}

void SerializeGauge(std::ostream& out, const MetricFamily& family,
                    const ClientMetric& metric) {
  WriteHead(out, family, metric);
  WriteValue(out, metric.gauge.value);
  WriteTail(out, metric);
}

void SerializeSummary(std::ostream& out, const MetricFamily& family,
                      const ClientMetric& metric) {
  auto& sum = metric.summary;
  WriteHead(out, family, metric, "_count");
  out << sum.sample_count;
  WriteTail(out, metric);

  WriteHead(out, family, metric, "_sum");
  WriteValue(out, sum.sample_sum);
  WriteTail(out, metric);

  for (auto& q : sum.quantile) {
    WriteHead(out, family, metric, "", "quantile", q.quantile);
    WriteValue(out, q.value);
    WriteTail(out, metric);
  }
}

void SerializeUntyped(std::ostream& out, const MetricFamily& family,
                      const ClientMetric& metric) {
  WriteHead(out, family, metric);
  WriteValue(out, metric.untyped.value);
  WriteTail(out, metric);
}

void SerializeHistogram(std::ostream& out, const MetricFamily& family,
                        const ClientMetric& metric) {
  auto& hist = metric.histogram;
  WriteHead(out, family, metric, "_count");
  out << hist.sample_count;
  WriteTail(out, metric);

  WriteHead(out, family, metric, "_sum");
  WriteValue(out, hist.sample_sum);
  WriteTail(out, metric);

  double last = -std::numeric_limits<double>::infinity();
  for (auto& b : hist.bucket) {
    WriteHead(out, family, metric, "_bucket", "le", b.upper_bound);
    last = b.upper_bound;
    out << b.cumulative_count;
    WriteTail(out, metric);
  }

  if (last != std::numeric_limits<double>::infinity()) {
    WriteHead(out, family, metric, "_bucket", "le", "+Inf");
    out << hist.sample_count;
    WriteTail(out, metric);
  }
}

void SerializeFamily(std::ostream& out, const MetricFamily& family) {
  const auto ends_with = [](const std::string& value,
                            const std::string& ending) -> bool {
    if (ending.size() > value.size()) {
      return false;
    }
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
  };

  const auto remove_suffix = [&ends_with](std::string& value,
                                          const std::string& suffix) {
    if (ends_with(value, suffix)) {
      value.erase(value.end() - suffix.size(), value.end());
    }
  };

  const auto compare_metrics = [](const ClientMetric& a,
                                  const ClientMetric& b) {
    return std::tie(a.label, a.timestamp) < std::tie(b.label, b.timestamp);
  };

  MetricFamily sorted_family{family};
  std::stable_sort(sorted_family.metric.begin(), sorted_family.metric.end(),
                   compare_metrics);

  if (sorted_family.type == MetricType::Counter) {
    remove_suffix(sorted_family.name, "_total");
  }

  if (!sorted_family.help.empty()) {
    out << "# HELP " << sorted_family.name << " " << sorted_family.help << "\n";
  }

  switch (sorted_family.type) {
    case MetricType::Counter: {
      out << "# TYPE " << sorted_family.name << " counter\n";
      for (const auto& metric : sorted_family.metric) {
        SerializeCounter(out, sorted_family, metric);
      }
      break;
    }
    case MetricType::Gauge:
      out << "# TYPE " << sorted_family.name << " gauge\n";
      for (auto& metric : sorted_family.metric) {
        SerializeGauge(out, sorted_family, metric);
      }
      break;
    case MetricType::Summary:
      out << "# TYPE " << sorted_family.name << " summary\n";
      for (auto& metric : sorted_family.metric) {
        SerializeSummary(out, sorted_family, metric);
      }
      break;
    case MetricType::Untyped:
      out << "# TYPE " << sorted_family.name << " unknown\n";
      for (auto& metric : sorted_family.metric) {
        SerializeUntyped(out, sorted_family, metric);
      }
      break;
    case MetricType::Histogram:
      out << "# TYPE " << sorted_family.name << " histogram\n";
      for (auto& metric : sorted_family.metric) {
        SerializeHistogram(out, sorted_family, metric);
      }
      break;
  }
}
}  // namespace

void TextSerializer::Serialize(std::ostream& out,
                               const std::vector<MetricFamily>& metrics) const {
  auto saved_locale = out.getloc();
  auto saved_precision = out.precision();

  out.imbue(std::locale::classic());
  out.precision(std::numeric_limits<double>::max_digits10 - 1);

  for (auto& family : metrics) {
    SerializeFamily(out, family);
  }

  out << "# EOF\n";

  out.imbue(saved_locale);
  out.precision(saved_precision);
}

}  // namespace prometheus
