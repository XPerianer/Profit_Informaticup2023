#pragma once

#include <variant>

#include "geometry/rectangle.hpp"
#include "geometry/vector.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

using geometry::Rectangle;
using geometry::Vector;

struct Deposit {
  Vector handle;
  Vector dimensions;

  ResourceType type{};
  bool operator==(const Deposit& other) const = default;
};

struct Obstacle {
  Vector handle;
  Vector dimensions;
  bool operator==(const Obstacle& other) const = default;
};

using LandscapeObject = std::variant<Deposit, Obstacle>;

inline Rectangle as_rectangle(const Deposit& deposit) {
  return Rectangle::from_top_left_and_dimensions(deposit.handle, deposit.dimensions);
}

inline Rectangle as_rectangle(const Obstacle& obstacle) {
  return Rectangle::from_top_left_and_dimensions(obstacle.handle, obstacle.dimensions);
}