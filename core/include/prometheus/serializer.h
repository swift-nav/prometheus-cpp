#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "prometheus/detail/core_export.h"
#include "prometheus/metric_family.h"

namespace prometheus {

class PROMETHEUS_CPP_CORE_EXPORT Serializer {
 public:
  virtual ~Serializer() = default;
  Serializer(const Serializer&) = default;
  Serializer& operator=(const Serializer&) = default;
  Serializer(Serializer&&) = default;
  Serializer& operator=(Serializer&&) = default;
  virtual std::string Serialize(const std::vector<MetricFamily>& metrics) const;
  virtual void Serialize(std::ostream& out,
                         const std::vector<MetricFamily>& metrics) const = 0;

 protected:
  Serializer() = default;
};

}  // namespace prometheus
