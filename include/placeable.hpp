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
  constexpr static size_t OCCUPIED_CELL_COUNT = 7;
  Vec2 handle;
  Rotation rotation{};

  bool operator==(const Combiner& other) const = default;

  static Combiner with_left_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return Vec2{coordinate.x() + 1, coordinate.y() + 1};
        case Rotation::UP_TO_DOWN:
          return Vec2{coordinate.x() - 1, coordinate.y() + 1};
        case Rotation::RIGHT_TO_LEFT:
          return Vec2{coordinate.x() - 1, coordinate.y() - 1};
        case Rotation::DOWN_TO_UP:
          return Vec2{coordinate.x() + 1, coordinate.y() - 1};
        default:
          FAIL("Unexpected control flow.");
      }
    }();
    return Combiner{handle, rotation};
  }

  static Combiner with_right_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return Vec2{coordinate.x() + 1, coordinate.y() - 1};
        case Rotation::UP_TO_DOWN:
          return Vec2{coordinate.x() + 1, coordinate.y() + 1};
        case Rotation::RIGHT_TO_LEFT:
          return Vec2{coordinate.x() - 1, coordinate.y() + 1};
        case Rotation::DOWN_TO_UP:
          return Vec2{coordinate.x() - 1, coordinate.y() - 1};
        default:
          FAIL("Unexpected control flow.");
      }
    }();
    return Combiner{handle, rotation};
  }

  [[nodiscard]] constexpr Vec2 egress() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return handle + Vec2{1, 0};
      case Rotation::UP_TO_DOWN:
        return handle + Vec2{0, 1};
      case Rotation::RIGHT_TO_LEFT:
        return handle + Vec2{-1, 0};
      case Rotation::DOWN_TO_UP:
        return handle + Vec2{0, -1};
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::vector<Vec2> ingresses() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return {handle + Vec2{-1, -1}, handle + Vec2{-1, 0}, handle + Vec2{-1, 1}};
      case Rotation::UP_TO_DOWN:
        return {handle + Vec2{-1, -1}, handle + Vec2{0, -1}, handle + Vec2{1, -1}};
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{1, -1}, handle + Vec2{1, 0}, handle + Vec2{1, 1}};
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{-1, 1}, handle + Vec2{0, 1}, handle + Vec2{1, 1}};
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells()
      const {  // todo: duplicate to mine
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
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return {handle + Vec2{-1, -1}, handle + Vec2{0, -1}, handle + Vec2{-1, 0},
                handle + Vec2{0, 0},   handle + Vec2{1, 0},  handle + Vec2{-1, 1},
                handle + Vec2{0, 1}};
      case Rotation::UP_TO_DOWN:
        return {handle + Vec2{-1, -1}, handle + Vec2{0, -1}, handle + Vec2{1, -1},
                handle + Vec2{-1, 0},  handle + Vec2{0, 0},  handle + Vec2{1, 0},
                handle + Vec2{0, 1}};
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{0, -1}, handle + Vec2{1, -1}, handle + Vec2{-1, 0},
                handle + Vec2{0, 0},  handle + Vec2{1, 0},  handle + Vec2{0, 1},
                handle + Vec2{1, 1}};
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{0, -1}, handle + Vec2{-1, 0}, handle + Vec2{0, 0},
                handle + Vec2{1, 0},  handle + Vec2{-1, 1}, handle + Vec2{0, 1},
                handle + Vec2{1, 1}};
      default:
        FAIL("Unexpected control flow.");
    }
  }
};

struct Conveyor3 {
  constexpr static size_t OCCUPIED_CELL_COUNT = 3;
  Vec2 handle;
  Rotation rotation{};

  bool operator==(const Conveyor3& other) const = default;

  static Conveyor3 with_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return Vec2{coordinate.x() + 1, coordinate.y()};
        case Rotation::UP_TO_DOWN:
          return Vec2{coordinate.x(), coordinate.y() + 1};
        case Rotation::RIGHT_TO_LEFT:
          return Vec2{coordinate.x() - 1, coordinate.y()};
        case Rotation::DOWN_TO_UP:
          return Vec2{coordinate.x(), coordinate.y() - 1};
        default:
          FAIL("Unexpected control flow.");
      }
    }();
    return Conveyor3{handle, rotation};
  }

  [[nodiscard]] constexpr Vec2 egress() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return handle + Vec2{1, 0};
      case Rotation::UP_TO_DOWN:
        return handle + Vec2{0, 1};
      case Rotation::RIGHT_TO_LEFT:
        return handle + Vec2{-1, 0};
      case Rotation::DOWN_TO_UP:
        return handle + Vec2{0, -1};
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells()
      const {  // todo: duplicate to mine
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
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{-1, 0}, handle + Vec2{0, 0},
                handle + Vec2{1, 0}};  // would Rectangle be a better choice here?
      case Rotation::UP_TO_DOWN:
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{0, -1}, handle + Vec2{0, 0}, handle + Vec2{0, 1}};
      default:
        FAIL("Unexpected control flow.");
    }
  }
};

struct Conveyor4 {
  constexpr static size_t OCCUPIED_CELL_COUNT = 4;
  Vec2 handle;
  Rotation rotation{};

  bool operator==(const Conveyor4& other) const = default;

  static Conveyor4 with_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return Vec2{coordinate.x() + 1, coordinate.y()};
        case Rotation::UP_TO_DOWN:
          return Vec2{coordinate.x(), coordinate.y() + 1};
        case Rotation::RIGHT_TO_LEFT:
          return Vec2{coordinate.x() - 2, coordinate.y()};
        case Rotation::DOWN_TO_UP:
          return Vec2{coordinate.x(), coordinate.y() - 2};
        default:
          FAIL("Unexpected control flow.");
      }
    }();
    return Conveyor4{handle, rotation};
  }

  [[nodiscard]] constexpr Vec2 egress() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return handle + Vec2{2, 0};
      case Rotation::UP_TO_DOWN:
        return handle + Vec2{0, 2};
      case Rotation::RIGHT_TO_LEFT:
        return handle + Vec2{-1, 0};
      case Rotation::DOWN_TO_UP:
        return handle + Vec2{0, -1};
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells()
      const {  // todo: duplicate to mine
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
      default:
        FAIL("Unexpected control flow.");
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{-1, 0}, handle + Vec2{0, 0}, handle + Vec2{1, 0},
                handle + Vec2{2, 0}};  // would Rectangle be a better choice here?
      case Rotation::UP_TO_DOWN:
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{0, -1}, handle + Vec2{0, 0}, handle + Vec2{0, 1},
                handle + Vec2{0, 2}};
      default:
        FAIL("Unexpected control flow.");
    }
  }
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
  constexpr static size_t OCCUPIED_CELL_COUNT = 6;
  Vec2 handle;
  Rotation rotation{};

  bool operator==(const Mine& other) const = default;

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
        default:
          FAIL("Unexpected control flow.");
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
      default:
        FAIL("Unexpected control flow.");
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
      default:
        FAIL("Unexpected control flow.");
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
      default:
        FAIL("Unexpected control flow.");
    }
  }
};

using PlaceableObject = std::variant<Combiner, Conveyor3, Conveyor4, Factory, Mine>;
}  // namespace profit
