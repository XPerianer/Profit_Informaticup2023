#pragma once

#include <array>
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
  constexpr static Vec2 DIMENSIONS{5, 5};
  Vec2 handle;
  FactoryType type{};
  bool operator==(const Factory& other) const = default;
};

inline Rectangle as_rectangle(const Factory& factory) {
  return Rectangle::from_top_left_and_dimensions(factory.handle, Factory::DIMENSIONS);
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

  constexpr static size_t OCCUPIED_CELL_COUNT = 6;

  static Mine with_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return Vec2{coordinate.x() + 1, coordinate.y() - 1};
        case Rotation::UP_TO_DOWN:
          return Vec2{coordinate.x(), coordinate.y() + 1};
        case Rotation::RIGHT_TO_LEFT:
          return Vec2{coordinate.x() - 2, coordinate.y()};
        case Rotation::DOWN_TO_UP:
          return Vec2{coordinate.x() - 1, coordinate.y() - 2};
      }
    }();
    return Mine{handle, rotation};
  }

  [[nodiscard]] constexpr Vec2 egress() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return handle + Vec2{2, 1};
      case Rotation::UP_TO_DOWN:
        return handle + Vec2{0, 2};
      case Rotation::RIGHT_TO_LEFT:
        return handle + Vec2{-1, 0};
      case Rotation::DOWN_TO_UP:
        return handle + Vec2{1, -1};
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells() const {
    Vec2 top = egress() + Vec2{0, -1};
    Vec2 right = egress() + Vec2{1, 0};
    Vec2 down = egress() + Vec2{0, 1};
    Vec2 left = egress() + Vec2{-1, 0};

    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return {top, right, down};
      case Rotation::UP_TO_DOWN:
        return {left, right, down};
      case Rotation::RIGHT_TO_LEFT:
        return {top, left, down};
      case Rotation::DOWN_TO_UP:
        return {top, left, right};
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return {handle + Vec2{0, 0}, handle + Vec2{1, 0}, handle + Vec2{-1, 1},
                handle + Vec2{0, 1}, handle + Vec2{1, 1}, handle + Vec2{2, 1}};
      case Rotation::UP_TO_DOWN:
        return {handle + Vec2{0, -1}, handle + Vec2{0, 0}, handle + Vec2{1, 0},
                handle + Vec2{0, 1},  handle + Vec2{1, 1}, handle + Vec2{0, 2}};
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{-1, 0}, handle + Vec2{0, 0}, handle + Vec2{1, 0},
                handle + Vec2{2, 0},  handle + Vec2{0, 1}, handle + Vec2{1, 1}};
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{1, -1}, handle + Vec2{0, 0}, handle + Vec2{1, 0},
                handle + Vec2{0, 1},  handle + Vec2{1, 1}, handle + Vec2{1, 2}};
    }
  }
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
}  // namespace profit
