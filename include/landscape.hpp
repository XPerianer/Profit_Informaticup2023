#pragma once

#include <variant>

#include "constants/subtype.hpp"
#include "data_structures/vec2.hpp"

struct Deposit {
  Vec2 handle;
  Vec2 dimensions;

  Subtype subtype{};
  bool operator==(const Deposit& other) const = default;
};

struct Obstacle {
  Vec2 handle;
  Vec2 dimensions;
  bool operator==(const Obstacle& other) const = default;
};

using LandscapeObject = std::variant<Deposit, Obstacle>;