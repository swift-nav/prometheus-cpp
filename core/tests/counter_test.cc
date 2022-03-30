#include "prometheus/counter.h"

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(CounterTest, initializeWithZero) {
  Counter counter;
  EXPECT_EQ(counter.Value(), 0);
}

TEST(CounterTest, inc) {
  Counter counter;
  counter.Increment();
  EXPECT_EQ(counter.Value(), 1.0);
}

TEST(CounterTest, incNumber) {
  Counter counter;
  counter.Increment(4);
  EXPECT_EQ(counter.Value(), 4.0);
}

TEST(CounterTest, incMultiple) {
  Counter counter;
  counter.Increment();
  counter.Increment();
  counter.Increment(5);
  EXPECT_EQ(counter.Value(), 7.0);
}

TEST(CounterTest, incNegativeValue) {
  Counter counter;
  counter.Increment(5.0);
  counter.Increment(-5.0);
  EXPECT_EQ(counter.Value(), 5.0);
}

}  // namespace
}  // namespace prometheus
