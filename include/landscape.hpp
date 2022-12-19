#pragma once

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <variant>
#include <vector>

#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

namespace profit {

using geometry::Rectangle;
using geometry::Vec2;
using DepositId = int16_t;
using ConnectedComponent = std::vector<DepositId>;

struct Deposit {
  Vec2 handle;
  Vec2 dimensions;

  ResourceType type{};
  bool operator==(const Deposit& other) const = default;
};

struct Obstacle {
  Vec2 handle;
  Vec2 dimensions;
  bool operator==(const Obstacle& other) const = default;
};

using LandscapeObject = std::variant<Deposit, Obstacle>;

inline Rectangle as_rectangle(const Deposit& deposit) {
  return Rectangle::from_top_left_and_dimensions(deposit.handle, deposit.dimensions);
}

inline Rectangle as_rectangle(const Obstacle& obstacle) {
  return Rectangle::from_top_left_and_dimensions(obstacle.handle, obstacle.dimensions);
}

inline std::vector<ConnectedComponent> connected_components_from(
    const std::vector<Deposit>& deposits, const std::vector<std::vector<Vec2>>& reached_egresses) {
  ConnectedComponent uncategorized(deposits.size());
  std::iota(uncategorized.begin(), uncategorized.end(), 0);
  std::vector<ConnectedComponent> connected_components;

  while (!uncategorized.empty()) {
    DepositId categorized = uncategorized.back();
    uncategorized.pop_back();
    ConnectedComponent connected_component = {categorized};

    for (auto i = uncategorized.rbegin(); i != uncategorized.rend(); ++i) {
      auto candidate = *i;
      const Rectangle candidate_shape = as_rectangle(deposits[candidate]);
      bool reaches_candidate =
          std::ranges::any_of(reached_egresses[categorized], [&](auto reached_egress) {
            return geometry::is_on_border(candidate_shape, reached_egress);
          });

      if (reaches_candidate) {
        connected_component.emplace_back(candidate);
        uncategorized.erase(std::next(i).base());
      }
    }
    connected_components.emplace_back(connected_component);
  }
  return connected_components;
}

}  // namespace profit