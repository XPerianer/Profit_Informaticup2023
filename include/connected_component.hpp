#pragma once

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "fields/component_map.hpp"
#include "geometry/vec2.hpp"
#include "landscape.hpp"

namespace profit {

using ConnectedComponent = std::vector<DepositId>;

class ConnectedComponentsWrapper {
 private:
  std::vector<DepositId> components_;
  std::vector<int> sizes_;
  DepositId elements_;
  int count_;
  ComponentMap map_;

 public:
  explicit ConnectedComponentsWrapper(DepositId count, Vec2 field_dimensions)
      : elements_(count), count_(count), map_(field_dimensions) {
    for (DepositId i = 0; i < count; ++i) {
      components_.emplace_back(i);
      sizes_.emplace_back(1);
    }
  }

  DepositId find(DepositId deposit) {
    while (deposit != components_[deposit]) {
      components_[deposit] = components_[components_[deposit]];
      deposit = components_[deposit];
    }
    return deposit;
  }

  void set_reachable(const DepositId deposit, Vec2 coordinate) {
    if (map_.at(coordinate) == NO_COMPONENT) {
      map_.set(coordinate, deposit);
      return;
    }
    DepositId root_a = find(deposit);
    DepositId root_b = find(map_.at(coordinate));
    if (root_a == root_b) {
      return;
    }
    if (sizes_[root_a] < sizes_[root_b]) {
      components_[root_a] = root_b;
      sizes_[root_b] += sizes_[root_a];
    } else {
      components_[root_b] = root_a;
      sizes_[root_a] += sizes_[root_b];
    }
    count_--;
  }

  bool connected(DepositId deposit_a, DepositId deposit_b) {
    return find(deposit_a) == find(deposit_b);
  }

  std::vector<ConnectedComponent> extract() {
    std::vector<ConnectedComponent> connected_components;
    connected_components.reserve(count_);
    std::vector<bool> added(elements_, false);

    for (DepositId deposit = 0; deposit < elements_; deposit++) {
      if (added[deposit]) {
        continue;
      }

      ConnectedComponent connected_component;
      for (DepositId candidate = deposit; candidate < elements_; candidate++) {
        if (added[candidate] || !connected(deposit, candidate)) {
          continue;
        }
        connected_component.emplace_back(candidate);
        added[candidate] = true;
      }
      connected_components.emplace_back(connected_component);
    }

    return connected_components;
  }
};

}  // namespace profit
