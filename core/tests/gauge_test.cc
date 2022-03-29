#include "prometheus/gauge.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace prometheus {
namespace {

TEST(GaugeTest, initialize_with_zero) {
  Gauge gauge;
  EXPECT_EQ(gauge.Value(), 0);
}

TEST(GaugeTest, inc) {
  Gauge gauge;
  gauge.Increment();
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, inc_number) {
  Gauge gauge;
  gauge.Increment(4);
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, inc_multiple) {
  Gauge gauge;
  gauge.Increment();
  gauge.Increment();
  gauge.Increment(5);
  EXPECT_EQ(gauge.Value(), 7.0);
}

TEST(GaugeTest, inc_negative_value) {
  Gauge gauge;
  gauge.Increment(-1.0);
  EXPECT_EQ(gauge.Value(), -1.0);
}

TEST(GaugeTest, dec) {
  Gauge gauge;
  gauge.Set(5.0);
  gauge.Decrement();
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, dec_negative_value) {
  Gauge gauge;
  gauge.Decrement(-1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, dec_number) {
  Gauge gauge;
  gauge.Set(5.0);
  gauge.Decrement(3.0);
  EXPECT_EQ(gauge.Value(), 2.0);
}

TEST(GaugeTest, set) {
  Gauge gauge;
  gauge.Set(3.0);
  EXPECT_EQ(gauge.Value(), 3.0);
}

TEST(GaugeTest, set_multiple) {
  Gauge gauge;
  gauge.Set(3.0);
  gauge.Set(8.0);
  gauge.Set(1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, set_to_current_time) {
  Gauge gauge;
  gauge.SetToCurrentTime();
  EXPECT_GT(gauge.Value(), 0.0);
}

TEST(GaugeTest, set_expired) {
  Gauge gauge;
  gauge.Set(1.0);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_TRUE(
      gauge.Expired(std::chrono::system_clock::now(), std::chrono::seconds(1)));
}

TEST(GaugeTest, set_not_expired) {
  Gauge gauge;
  gauge.Set(1.0);
  EXPECT_FALSE(
      gauge.Expired(std::chrono::system_clock::now(), std::chrono::seconds(1)));
}

TEST(GaugeTest, increment_expired) {
  Gauge gauge;
  gauge.Increment();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_TRUE(
      gauge.Expired(std::chrono::system_clock::now(), std::chrono::seconds(1)));
}

TEST(GaugeTest, increment_not_expired) {
  Gauge gauge;
  gauge.Increment();
  EXPECT_FALSE(
      gauge.Expired(std::chrono::system_clock::now(), std::chrono::seconds(1)));
}

}  // namespace
}  // namespace prometheus
