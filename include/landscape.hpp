#pragma once

#include <variant>

#include "utils.hpp"

struct Deposit {
  Vec2 handle;
  Vec2 dimensions;

  uint8_t subtype;
};

struct Obstacle {
  Vec2 handle;
  Vec2 dimensions;
};

using LandscapeObject = std::variant<Deposit, Obstacle>;