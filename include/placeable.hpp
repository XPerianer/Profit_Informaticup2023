#pragma once

#include <variant>

#include "constants/rotation.hpp"
#include "constants/subtype.hpp"
#include "data_structures/Vec2.hpp"

struct Combiner {
  Vec2 handle;
  Rotation rotation{};
  bool operator==(const Combiner& other) const = default;
};

struct Conveyor3 {
  Vec2 handle;
  Rotation rotation{};
  bool operator==(const Conveyor3& other) const = default;
};

struct Conveyor4 {
  Vec2 handle;
  Rotation rotation{};
  bool operator==(const Conveyor4& other) const = default;
};

struct Factory {
  Vec2 handle;
  Subtype subtype;
  bool operator==(const Factory& other) const = default;
};

struct Mine {
  Vec2 handle;
  Rotation rotation{};
  bool operator==(const Mine& other) const = default;
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
