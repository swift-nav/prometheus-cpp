#include "prometheus/check_names.h"

#include <gtest/gtest.h>

namespace prometheus {
namespace {

TEST(CheckNamesTest, emptyMetricName) { EXPECT_FALSE(CheckMetricName("")); }
TEST(CheckNamesTest, goodMetricName) {
  EXPECT_TRUE(CheckMetricName("prometheus_notifications_total"));
}
TEST(CheckNamesTest, reservedMetricName) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_metric"));
}
TEST(CheckNamesTest, malformedMetricName) {
  EXPECT_FALSE(CheckMetricName("fa mi ly with space in name or |"));
}
TEST(CheckNamesTest, emptyLabelName) { EXPECT_FALSE(CheckLabelName("")); }
TEST(CheckNamesTest, invalidLabelName) {
  EXPECT_FALSE(CheckLabelName("log-level"));
}
TEST(CheckNamesTest, leadingInvalidLabelName) {
  EXPECT_FALSE(CheckLabelName("-abcd"));
}
TEST(CheckNamesTest, trailingInvalidLabelName) {
  EXPECT_FALSE(CheckLabelName("abcd-"));
}
TEST(CheckNamesTest, goodLabelName) { EXPECT_TRUE(CheckLabelName("type")); }
TEST(CheckNamesTest, reservedLabelName) {
  EXPECT_FALSE(CheckMetricName("__some_reserved_label"));
}

}  // namespace
}  // namespace prometheus
