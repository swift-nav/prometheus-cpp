#pragma once

#include <chrono>
#include <string>

#include "prometheus/labels.h"

// IWYU pragma: private
// IWYU pragma: no_include "prometheus/family.h"

namespace prometheus {

template <typename T>
class Family;    // IWYU pragma: keep
class Registry;  // IWYU pragma: keep

namespace detail {

template <typename T>
class Builder {
 public:
  Builder& Labels(const ::prometheus::Labels& labels);
  Builder& Name(const std::string&);
  Builder& Help(const std::string&);
  Builder& TTL(const std::chrono::seconds& ttl);
  Family<T>& Register(Registry&);

 private:
  ::prometheus::Labels labels_;
  std::string name_;
  std::string help_;
  std::chrono::seconds ttl_{std::chrono::seconds::max()};
};

}  // namespace detail
}  // namespace prometheus
