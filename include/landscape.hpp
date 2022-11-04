#pragma once

#include <variant>

#include "utils.hpp"

struct Deposit {
  Vec2 handle;
  Vec2 dimensions;

  uint8_t subtype;
  bool operator==(const Deposit& other) const = default;
};

struct Obstacle {
  Vec2 handle;
  Vec2 dimensions;
  bool operator==(const Obstacle& other) const = default;
};

using LandscapeObject = std::variant<Deposit, Obstacle>;