#pragma once

#include <variant>

#include "resource_type.hpp"
#include "utils.hpp"

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