#pragma once

#include <variant>

#include "geometry/vector.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

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