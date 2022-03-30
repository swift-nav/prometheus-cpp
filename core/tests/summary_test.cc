#include "prometheus/summary.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <thread>

namespace prometheus {
namespace {

TEST(SummaryTest, initializeWithZero) {
  Summary summary{Summary::Quantiles{}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 0U);
  EXPECT_EQ(s.sample_sum, 0);
}

TEST(SummaryTest, sampleCount) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(200);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_count, 2U);
}

TEST(SummaryTest, sampleSum) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}}};
  summary.Observe(0);
  summary.Observe(1);
  summary.Observe(101);
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.sample_sum, 102);
}

TEST(SummaryTest, quantileSize) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  EXPECT_EQ(s.quantile.size(), 2U);
}

TEST(SummaryTest, quantileBounds) {
  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.90, 0.01}, {0.99, 0.001}}};
  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);
  EXPECT_DOUBLE_EQ(s.quantile.at(0).quantile, 0.5);
  EXPECT_DOUBLE_EQ(s.quantile.at(1).quantile, 0.9);
  EXPECT_DOUBLE_EQ(s.quantile.at(2).quantile, 0.99);
}

TEST(SummaryTest, quantileValues) {
  static const int samples = 1000000;

  Summary summary{Summary::Quantiles{{0.5, 0.05}, {0.9, 0.01}, {0.99, 0.001}}};
  for (int i = 1; i <= samples; ++i) {
    summary.Observe(i);
  }

  auto metric = summary.Collect();
  auto s = metric.summary;
  ASSERT_EQ(s.quantile.size(), 3U);

  EXPECT_NEAR(s.quantile.at(0).value, 0.5 * samples, 0.05 * samples);
  EXPECT_NEAR(s.quantile.at(1).value, 0.9 * samples, 0.01 * samples);
  EXPECT_NEAR(s.quantile.at(2).value, 0.99 * samples, 0.001 * samples);
}

TEST(SummaryTest, maxAge) {
  Summary summary{Summary::Quantiles{{0.99, 0.001}}, std::chrono::seconds(1),
                  2};
  summary.Observe(8.0);

  const auto test_value = [&summary](double ref) {
    auto metric = summary.Collect();
    auto s = metric.summary;
    ASSERT_EQ(s.quantile.size(), 1U);

    if (std::isnan(ref)) {
      EXPECT_TRUE(std::isnan(s.quantile.at(0).value));
    } else {
      EXPECT_DOUBLE_EQ(s.quantile.at(0).value, ref);
    }
  };

  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(8.0);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  test_value(std::numeric_limits<double>::quiet_NaN());
}

TEST(SummaryTest, constructionWithDynamicQuantileVector) {
  auto quantiles = Summary::Quantiles{{0.99, 0.001}};
  quantiles.emplace_back(0.5, 0.05);

  Summary summary{quantiles, std::chrono::seconds(1), 2};
  summary.Observe(8.0);
}

}  // namespace
}  // namespace prometheus
