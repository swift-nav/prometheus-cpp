#include "prometheus/registry.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <tuple>

#include "prometheus/counter.h"
#include "prometheus/detail/future_std.h"
#include "prometheus/gauge.h"
#include "prometheus/histogram.h"
#include "prometheus/summary.h"

namespace prometheus {

namespace {
template <typename T>
void CollectAll(std::vector<MetricFamily>& results, const T& families,
                const std::chrono::system_clock::time_point& time) {
  for (auto&& collectable : families) {
    auto metrics = collectable->Collect(time);
    results.insert(results.end(), std::make_move_iterator(metrics.begin()),
                   std::make_move_iterator(metrics.end()));
  }
}

bool FamilyNameExists(const std::string& /* name */) { return false; }

template <typename T, typename... Args>
bool FamilyNameExists(const std::string& name, const T& families,
                      Args&&... args) {
  auto sameName = [&name](const typename T::value_type& entry) {
    return name == entry->GetName();
  };
  auto exists = std::find_if(std::begin(families), std::end(families),
                             sameName) != std::end(families);
  return exists || FamilyNameExists(name, args...);
}
}  // namespace

Registry::Registry(InsertBehavior insert_behavior)
    : insert_behavior_{insert_behavior} {}

Registry::~Registry() = default;

std::vector<MetricFamily> Registry::Collect() const {
  return Collect(std::chrono::system_clock::now());
}

std::vector<MetricFamily> Registry::Collect(
    const std::chrono::system_clock::time_point& time) const {
  std::lock_guard<std::mutex> lock{mutex_};
  auto results = std::vector<MetricFamily>{};

  CollectAll(results, counters_, time);
  CollectAll(results, gauges_, time);
  CollectAll(results, histograms_, time);
  CollectAll(results, summaries_, time);

  return results;
}

template <>
std::vector<std::unique_ptr<Family<Counter>>>& Registry::GetFamilies() {
  return counters_;
}

template <>
std::vector<std::unique_ptr<Family<Gauge>>>& Registry::GetFamilies() {
  return gauges_;
}

template <>
std::vector<std::unique_ptr<Family<Histogram>>>& Registry::GetFamilies() {
  return histograms_;
}

template <>
std::vector<std::unique_ptr<Family<Summary>>>& Registry::GetFamilies() {
  return summaries_;
}

template <>
bool Registry::NameExistsInOtherType<Counter>(const std::string& name) const {
  return FamilyNameExists(name, gauges_, histograms_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Gauge>(const std::string& name) const {
  return FamilyNameExists(name, counters_, histograms_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Histogram>(const std::string& name) const {
  return FamilyNameExists(name, counters_, gauges_, summaries_);
}

template <>
bool Registry::NameExistsInOtherType<Summary>(const std::string& name) const {
  return FamilyNameExists(name, counters_, gauges_, histograms_);
}

template <typename T>
Family<T>& Registry::Add(const std::string& name, const std::string& help,
                         const Labels& labels,
                         const std::chrono::seconds& ttl) {
  std::lock_guard<std::mutex> lock{mutex_};

  if (NameExistsInOtherType<T>(name)) {
    std::stringstream ss;
    ss << "Family name already exists with different type (name:'" << name
       << "',type:'" << typeid(T).name() << "');help:" << help << ";labels:";
    for (const auto& label : labels) {
      ss << label.first << "=" << label.second << ",";
    }
    ss << std::endl;
    std::cerr << ss.str();
    throw std::invalid_argument(ss.str());
  }

  auto& families = GetFamilies<T>();

  if (insert_behavior_ == InsertBehavior::Merge) {
    auto same_name_and_labels =
        [&name, &labels](const std::unique_ptr<Family<T>>& family) {
          return std::tie(name, labels) ==
                 std::tie(family->GetName(), family->GetConstantLabels());
        };

    auto it =
        std::find_if(families.begin(), families.end(), same_name_and_labels);
    if (it != families.end()) {
      return **it;
    }
  }

  auto same_name = [&name](const std::unique_ptr<Family<T>>& family) {
    return name == family->GetName();
  };

  auto it = std::find_if(families.begin(), families.end(), same_name);
  if (it != families.end()) {
    std::stringstream ss;
    ss << "Family name already exists (name:'" << name << "',type:'"
       << typeid(T).name() << "');help:" << help << ";labels:";
    for (const auto& label : labels) {
      ss << label.first << "=" << label.second << ",";
    }
    ss << std::endl;
    std::cerr << ss.str();
    throw std::invalid_argument(ss.str());
  }

  auto family = detail::make_unique<Family<T>>(name, help, labels, ttl);
  auto& ref = *family;
  families.push_back(std::move(family));
  return ref;
}

template Family<Counter>& Registry::Add(const std::string& name,
                                        const std::string& help,
                                        const Labels& labels,
                                        const std::chrono::seconds& ttl);

template Family<Gauge>& Registry::Add(const std::string& name,
                                      const std::string& help,
                                      const Labels& labels,
                                      const std::chrono::seconds& ttl);

template Family<Summary>& Registry::Add(const std::string& name,
                                        const std::string& help,
                                        const Labels& labels,
                                        const std::chrono::seconds& ttl);

template Family<Histogram>& Registry::Add(const std::string& name,
                                          const std::string& help,
                                          const Labels& labels,
                                          const std::chrono::seconds& ttl);

template <typename T>
bool Registry::Remove(const Family<T>& family) {
  std::lock_guard<std::mutex> lock{mutex_};

  auto& families = GetFamilies<T>();
  auto same_family = [&family](const std::unique_ptr<Family<T>>& in) {
    return &family == in.get();
  };

  auto it = std::find_if(families.begin(), families.end(), same_family);
  if (it == families.end()) {
    return false;
  }

  families.erase(it);
  return true;
}

template bool PROMETHEUS_CPP_CORE_EXPORT
Registry::Remove(const Family<Counter>& family);

template bool PROMETHEUS_CPP_CORE_EXPORT
Registry::Remove(const Family<Gauge>& family);

template bool PROMETHEUS_CPP_CORE_EXPORT
Registry::Remove(const Family<Summary>& family);

template bool PROMETHEUS_CPP_CORE_EXPORT
Registry::Remove(const Family<Histogram>& family);

}  // namespace prometheus
