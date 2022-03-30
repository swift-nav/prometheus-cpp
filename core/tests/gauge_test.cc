#include "prometheus/gauge.h"

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(GaugeTest, initializeWithZero) {
  Gauge gauge;
  EXPECT_EQ(gauge.Value(), 0);
}

TEST(GaugeTest, inc) {
  Gauge gauge;
  gauge.Increment();
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, incNumber) {
  Gauge gauge;
  gauge.Increment(4);
  EXPECT_EQ(gauge.Value(), 4.0);
}

TEST(GaugeTest, incMultiple) {
  Gauge gauge;
  gauge.Increment();
  gauge.Increment();
  gauge.Increment(5);
  EXPECT_EQ(gauge.Value(), 7.0);
}

TEST(GaugeTest, incNegativeValue) {
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

TEST(GaugeTest, decNegativeValue) {
  Gauge gauge;
  gauge.Decrement(-1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, decNumber) {
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

TEST(GaugeTest, setMultiple) {
  Gauge gauge;
  gauge.Set(3.0);
  gauge.Set(8.0);
  gauge.Set(1.0);
  EXPECT_EQ(gauge.Value(), 1.0);
}

TEST(GaugeTest, setToCurrentTime) {
  Gauge gauge;
  gauge.SetToCurrentTime();
  EXPECT_GT(gauge.Value(), 0.0);
}

}  // namespace
}  // namespace prometheus
