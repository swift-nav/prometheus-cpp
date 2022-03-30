#include "prometheus/text_serializer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <limits>
#include <string>

#include "prometheus/client_metric.h"
#include "prometheus/histogram.h"
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"
#include "prometheus/summary.h"

namespace prometheus {
namespace {

class TextSerializerTest : public testing::Test {
 public:
  std::string Serialize(MetricType type) const {
    MetricFamily metricFamily;
    metricFamily.name = name;
    metricFamily.help = help;
    metricFamily.type = type;
    metricFamily.metric = std::vector<ClientMetric>{metric};

    std::vector<MetricFamily> families{metricFamily};

    return textSerializer.Serialize(families);
  }

  std::string name = "my_metric";
  std::string help = "my metric help text";
  ClientMetric metric;
  TextSerializer textSerializer;
};

TEST_F(TextSerializerTest, shouldSerializeGauge) {
  metric.gauge.value = 12.3;

  const auto serialized = Serialize(MetricType::Gauge);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " gauge\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 12.3"));
}

TEST_F(TextSerializerTest, shouldSerializeNotANumber) {
  metric.gauge.value = std::nan("");

  const auto serialized = Serialize(MetricType::Gauge);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " gauge\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " Nan"));
}

TEST_F(TextSerializerTest, shouldSerializeNegativeInfinity) {
  metric.gauge.value = -std::numeric_limits<double>::infinity();

  const auto serialized = Serialize(MetricType::Gauge);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " gauge\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " -Inf"));
}

TEST_F(TextSerializerTest, shouldSerializePositiveInfinity) {
  metric.gauge.value = std::numeric_limits<double>::infinity();

  const auto serialized = Serialize(MetricType::Gauge);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " gauge\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " +Inf"));
}

TEST_F(TextSerializerTest, shouldEscapeBackslash) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\\v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\\v\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeNewline) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\nv"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\nv\"}"));
}

TEST_F(TextSerializerTest, shouldEscapeDoubleQuote) {
  metric.label.resize(1, ClientMetric::Label{"k", "v\"v"});
  EXPECT_THAT(Serialize(MetricType::Gauge),
              testing::HasSubstr(name + "{k=\"v\\\"v\"}"));
}

TEST_F(TextSerializerTest, shouldSerializeUntyped) {
  metric.untyped.value = 64.0;

  const auto serialized = Serialize(MetricType::Untyped);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " unknown\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 64\n"));
}

TEST_F(TextSerializerTest, shouldSerializeTimestamp) {
  metric.gauge.value = 64.0;
  metric.timestamp = std::chrono::milliseconds(1234);

  const auto serialized = Serialize(MetricType::Gauge);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " gauge\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + " 64 1.234\n"));
}

TEST_F(TextSerializerTest, shouldSerializeHistogramWithNoBuckets) {
  metric.histogram.sample_count = 2;
  metric.histogram.sample_sum = 32.0;

  const auto serialized = Serialize(MetricType::Histogram);
  EXPECT_THAT(serialized,
              testing::HasSubstr("# TYPE " + name + " histogram\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 32\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_bucket{le=\"+Inf\"} 2"));
}

TEST_F(TextSerializerTest, shouldSerializeHistogram) {
  Histogram histogram{{1}};
  histogram.Observe(0);
  histogram.Observe(200);
  metric = histogram.Collect();

  const auto serialized = Serialize(MetricType::Histogram);
  EXPECT_THAT(serialized,
              testing::HasSubstr("# TYPE " + name + " histogram\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_bucket{le=\"1\"} 1\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr(name + "_bucket{le=\"+Inf\"} 2\n"));
}

TEST_F(TextSerializerTest, shouldSerializeSummary) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  metric = summary.Collect();

  const auto serialized = Serialize(MetricType::Summary);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " summary\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_count 2\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_sum 200\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "{quantile=\"0.5\"} 0\n"));
}

TEST_F(TextSerializerTest, shouldSerializeCounter) {
  name = "short";
  metric.counter.value = 1.0;

  const auto serialized = Serialize(MetricType::Counter);
  EXPECT_THAT(serialized, testing::HasSubstr("# TYPE " + name + " counter\n"));
  EXPECT_THAT(serialized,
              testing::HasSubstr("# HELP " + name + " " + help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(name + "_total 1\n"));
}

TEST_F(TextSerializerTest, shouldSerializeCounterWithSuffix) {
  const std::string original_name = name;
  name += "_total";
  metric.counter.value = 1.0;

  const auto serialized = Serialize(MetricType::Counter);
  EXPECT_THAT(serialized,
              testing::HasSubstr("# TYPE " + original_name + " counter\n"));
  EXPECT_THAT(serialized, testing::HasSubstr("# HELP " + original_name + " " +
                                             help + "\n"));
  EXPECT_THAT(serialized, testing::HasSubstr(original_name + "_total 1\n"));
}

TEST_F(TextSerializerTest, shouldSerializeNoHelp) {
  help.clear();

  EXPECT_THAT(
      Serialize(MetricType::Gauge),
      testing::Not(testing::HasSubstr("# HELP " + name + " " + help + "\n")));
}

TEST_F(TextSerializerTest, shouldSerializeEof) {
  EXPECT_THAT(Serialize(MetricType::Gauge), testing::EndsWith("# EOF\n"));
}

TEST_F(TextSerializerTest, shouldSortLabels) {
  ClientMetric metric1;
  metric1.label.emplace_back(ClientMetric::Label{"b", "bb"});
  metric1.gauge.value = 1;

  ClientMetric metric2;
  metric2.label.emplace_back(ClientMetric::Label{"a", "aa"});
  metric2.gauge.value = 1;

  MetricFamily metricFamily;
  metricFamily.name = name;
  metricFamily.help = help;
  metricFamily.type = MetricType::Gauge;
  metricFamily.metric = std::vector<ClientMetric>{metric1, metric2};

  std::vector<MetricFamily> families{metricFamily};

  const auto serialized = textSerializer.Serialize(families);
  EXPECT_THAT(serialized, testing::EndsWith(name + "{b=\"bb\"} 1\n# EOF\n"));
}

TEST_F(TextSerializerTest, shouldSortLabelsAndTime) {
  ClientMetric metric1;
  metric1.label.emplace_back(ClientMetric::Label{"b", "bb"});
  metric1.gauge.value = 2;
  metric1.timestamp = std::chrono::milliseconds(2000);

  ClientMetric metric2;
  metric2.label.emplace_back(ClientMetric::Label{"a", "aa"});
  metric2.gauge.value = 2;
  metric2.timestamp = std::chrono::milliseconds(2000);

  ClientMetric metric3;
  metric3.label.emplace_back(ClientMetric::Label{"b", "bb"});
  metric3.gauge.value = 1;
  metric3.timestamp = std::chrono::milliseconds(1000);

  ClientMetric metric4;
  metric4.label.emplace_back(ClientMetric::Label{"a", "aa"});
  metric4.gauge.value = 1;
  metric4.timestamp = std::chrono::milliseconds(1000);

  MetricFamily metricFamily;
  metricFamily.name = name;
  metricFamily.help = help;
  metricFamily.type = MetricType::Gauge;
  metricFamily.metric =
      std::vector<ClientMetric>{metric1, metric2, metric3, metric4};

  std::vector<MetricFamily> families{metricFamily};

  const auto serialized = textSerializer.Serialize(families);
  EXPECT_THAT(serialized, testing::EndsWith(name + "{b=\"bb\"} 1 1\n" + name +
                                            "{b=\"bb\"} 2 2\n# EOF\n"));
}

}  // namespace
}  // namespace prometheus
