#include "prometheus/counter.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace prometheus {
namespace {

TEST(CounterTest, initialize_with_zero) {
  Counter counter;
  EXPECT_EQ(counter.Value(), 0);
}

TEST(CounterTest, inc) {
  Counter counter;
  counter.Increment();
  EXPECT_EQ(counter.Value(), 1.0);
}

TEST(CounterTest, inc_number) {
  Counter counter;
  counter.Increment(4);
  EXPECT_EQ(counter.Value(), 4.0);
}

TEST(CounterTest, inc_multiple) {
  Counter counter;
  counter.Increment();
  counter.Increment();
  counter.Increment(5);
  EXPECT_EQ(counter.Value(), 7.0);
}

TEST(CounterTest, inc_negative_value) {
  Counter counter;
  counter.Increment(5.0);
  counter.Increment(-5.0);
  EXPECT_EQ(counter.Value(), 5.0);
}

TEST(CounterTest, not_expired) {
  Counter counter;
  counter.Increment();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_FALSE(counter.Expired(std::chrono::system_clock::now(),
                               std::chrono::seconds(1)));
}

}  // namespace
}  // namespace prometheus
