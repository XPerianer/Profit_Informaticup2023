#pragma once

#include <variant>

#include "utils.hpp"

struct Combiner {
  Vec2 handle;
  Rotation rotation;
  bool operator==(const Combiner& other) const = default;
};

struct Conveyor3 {
  Vec2 handle;
  Rotation rotation;
  bool operator==(const Conveyor3& other) const = default;
};

struct Conveyor4 {
  Vec2 handle;
  Rotation rotation;
  bool operator==(const Conveyor4& other) const = default;
};

struct Factory {
  using TypeT = uint8_t;
  Vec2 handle;
  TypeT subtype;
  bool operator==(const Factory& other) const = default;
};

struct Mine {
  Vec2 handle;
  Rotation rotation;
  bool operator==(const Mine& other) const = default;
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
