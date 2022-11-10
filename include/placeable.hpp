#pragma once

#include <variant>

#include "utils.hpp"

struct Combiner {
  Vec2 handle;
  Rotation rotation;
};

struct Conveyor {
  Vec2 handle;
  Rotation rotation;
};

struct Mine {
  Vec2 handle;
  Rotation rotation;
};

struct Factory {
  using TypeT = uint8_t;
  Vec2 handle;
  TypeT subtype;
};

using PlaceableObject = std::variant<Combiner, Conveyor, Factory, Mine>;
