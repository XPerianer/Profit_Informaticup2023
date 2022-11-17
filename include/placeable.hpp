#pragma once

#include <variant>

#include "utils.hpp"

struct Combiner {
  Vec2 handle;
  Rotation rotation;
};

struct Conveyor3 {
  Vec2 handle;
  Rotation rotation;
};

struct Conveyor4 {
  Vec2 handle;
  Rotation rotation;
};

struct Factory {
  using TypeT = uint8_t;
  Vec2 handle;
  TypeT subtype;
};

struct Mine {
  Vec2 handle;
  Rotation rotation;
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
