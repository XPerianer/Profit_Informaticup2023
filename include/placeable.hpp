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
  Vec2 handle;
  uint8_t subtype;
};

using PlaceableObject = std::variant<Combiner, Conveyor, Factory, Mine>;
