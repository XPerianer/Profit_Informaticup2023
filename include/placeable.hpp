#pragma once

#include <variant>

#include "assert.hpp"
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

  static Mine with_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle;
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        handle = {static_cast<CoordT>(coordinate.x() + 1), static_cast<CoordT>(coordinate.y() - 1)};
        break;
      case Rotation::UP_TO_DOWN:
        handle = {static_cast<CoordT>(coordinate.x()), static_cast<CoordT>(coordinate.y() + 1)};
        break;
      case Rotation::RIGHT_TO_LEFT:
        handle = {static_cast<CoordT>(coordinate.x() - 2), static_cast<CoordT>(coordinate.y())};
        break;
      case Rotation::DOWN_TO_UP:
        handle = {static_cast<CoordT>(coordinate.x() - 1), static_cast<CoordT>(coordinate.y() - 2)};
        break;
      default:
        FAIL("Unhandled rotation");
    }
    return Mine{handle, rotation};
  }
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
