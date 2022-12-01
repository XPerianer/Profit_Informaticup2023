#pragma once

#include <variant>

#include "assert.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "rotation.hpp"
#include "subtype.hpp"
#include "utils.hpp"

namespace profit {

using geometry::Rectangle;
using geometry::Vec2;

using FactoryType = Subtype;
constexpr size_t FACTORY_TYPE_COUNT = SUBTYPE_COUNT;

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
  FactoryType type{};
  bool operator==(const Factory& other) const = default;
};

inline Rectangle as_rectangle(const Factory& factory) {
  return Rectangle::from_top_left_and_dimensions(factory.handle, {5, 5});
}

// TODO when we need it
/*
inline Rectangle as_rectangle(const Conveyor3& conveyor) {
  return Rectangle::from_top_left_and_dimensions(obstacle.handle, obstacle.dimensions);
}
*/

struct Mine {
  Vec2 handle;
  Rotation rotation{};
  bool operator==(const Mine& other) const = default;

  static Mine with_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle;
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        handle = {coordinate.x() + 1, coordinate.y() - 1};
        break;
      case Rotation::UP_TO_DOWN:
        handle = {coordinate.x(), coordinate.y() + 1};
        break;
      case Rotation::RIGHT_TO_LEFT:
        handle = {coordinate.x() - 2, coordinate.y()};
        break;
      case Rotation::DOWN_TO_UP:
        handle = {coordinate.x() - 1, coordinate.y() - 2};
        break;
      default:
        FAIL("Unhandled rotation");
    }
    return Mine{handle, rotation};
  }
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
}
