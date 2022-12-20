#pragma once

#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>

#include "fields/field.hpp"
#include "geometry/vec2.hpp"
#include "landscape.hpp"

namespace profit {

using DepositId = int16_t;
constexpr DepositId NO_COMPONENT = -1;
using ComponentMap = Field<DepositId, NO_COMPONENT, NO_COMPONENT>;

using ConnectedComponent = std::vector<DepositId>;

class ConnectedComponentUnion {
 private:
  std::unordered_map<DepositId, DepositId> deposits_to_cc_;
  ComponentMap map_;

 public:
  explicit ConnectedComponentUnion(const DepositId count, Vec2 field_dimensions)
      : map_(field_dimensions) {
    deposits_to_cc_.reserve(count);
    for (DepositId i = 0; i < count; ++i) {
      deposits_to_cc_[i] = i;
    }
  }

  void set_reachable(const DepositId deposit, Vec2 coordinate) {
    if (map_.at(coordinate) == NO_COMPONENT) {
      map_.set(coordinate, deposits_to_cc_[deposit]);
    } else {
      deposits_to_cc_[deposit] = map_.at(coordinate);
    }
  }

  std::vector<ConnectedComponent> extract() {
    std::vector<ConnectedComponent> connected_components(deposits_to_cc_.size());
    for (const auto& [key, value] : deposits_to_cc_) {
      connected_components[value].emplace_back(key);
    }
    std::vector<ConnectedComponent> compressed;
    std::ranges::copy_if(connected_components, std::back_inserter(compressed),
                         [&](const ConnectedComponent& connected_component) {
                           return !connected_component.empty();
                         });
    return compressed;
  }
};

}  // namespace profit
