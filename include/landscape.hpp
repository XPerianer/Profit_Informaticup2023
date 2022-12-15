#pragma once

#include <variant>

#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

namespace profit {

using geometry::Rectangle;
using geometry::Vec2;

using StoredResourceCount = int32_t;

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

inline StoredResourceCount initial_resource_count(const Deposit& deposit) {
  constexpr StoredResourceCount STORED_RESOURCES_PER_CELL = 5;

  return static_cast<StoredResourceCount>(deposit.dimensions.width()) *
         static_cast<StoredResourceCount>(deposit.dimensions.height()) * STORED_RESOURCES_PER_CELL;
}
}  // namespace profit