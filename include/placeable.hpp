#pragma once

#include <array>
#include <variant>

#include "assert.hpp"
#include "geometry/coordinate.hpp"
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

[[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells(Vec2 egress,
                                                                     Rotation rotation) {
  Vec2 top = egress + Vec2{0, -1};
  Vec2 right = egress + Vec2{1, 0};
  Vec2 down = egress + Vec2{0, 1};
  Vec2 left = egress + Vec2{-1, 0};

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

struct Combiner {
  constexpr static size_t OCCUPIED_CELL_COUNT = 7;
  Vec2 handle;
  Rotation rotation{};

  bool operator==(const Combiner& other) const = default;

  static Combiner with_left_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return coordinate + Vec2{1, 1};
        case Rotation::UP_TO_DOWN:
          return coordinate + Vec2{-1, 1};
        case Rotation::RIGHT_TO_LEFT:
          return coordinate + Vec2{-1, -1};
        case Rotation::DOWN_TO_UP:
          return coordinate + Vec2{1, -1};
      }
    }();
    return Combiner{handle, rotation};
  }

  static Combiner with_right_ingress(Vec2 coordinate, Rotation rotation) {
    Vec2 handle = [&]() {
      switch (rotation) {
        case Rotation::LEFT_TO_RIGHT:
          return coordinate + Vec2{1, -1};
        case Rotation::UP_TO_DOWN:
          return coordinate + Vec2{1, 1};
        case Rotation::RIGHT_TO_LEFT:
          return coordinate + Vec2{-1, 1};
        case Rotation::DOWN_TO_UP:
          return coordinate + Vec2{-1, -1};
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
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> ingresses() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
        return {handle + Vec2{-1, -1}, handle + Vec2{-1, 0}, handle + Vec2{-1, 1}};
      case Rotation::UP_TO_DOWN:
        return {handle + Vec2{-1, -1}, handle + Vec2{0, -1}, handle + Vec2{1, -1}};
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{1, -1}, handle + Vec2{1, 0}, handle + Vec2{1, 1}};
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{-1, 1}, handle + Vec2{0, 1}, handle + Vec2{1, 1}};
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells() const {
    return ::profit::downstream_ingress_cells(egress(), rotation);
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
          return coordinate + Vec2{1, 0};
        case Rotation::UP_TO_DOWN:
          return coordinate + Vec2{0, 1};
        case Rotation::RIGHT_TO_LEFT:
          return coordinate + Vec2{-1, 0};
        case Rotation::DOWN_TO_UP:
          return coordinate + Vec2{0, -1};
      }
    }();
    return Conveyor3{handle, rotation};
  }

  [[nodiscard]] constexpr bool can_overlap_at(const Vec2 cell) const { return cell == handle; }

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
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells() const {
    return ::profit::downstream_ingress_cells(egress(), rotation);
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{-1, 0}, handle + Vec2{0, 0}, handle + Vec2{1, 0}};
      case Rotation::UP_TO_DOWN:
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{0, -1}, handle + Vec2{0, 0}, handle + Vec2{0, 1}};
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
          return coordinate + Vec2{1, 0};
        case Rotation::UP_TO_DOWN:
          return coordinate + Vec2{0, 1};
        case Rotation::RIGHT_TO_LEFT:
          return coordinate + Vec2{-2, 0};
        case Rotation::DOWN_TO_UP:
          return coordinate + Vec2{0, -2};
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
    }
  }

  [[nodiscard]] constexpr bool can_overlap_at(const Vec2 cell) const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
      case Rotation::RIGHT_TO_LEFT:
        return cell == handle || cell == handle + Vec2{1, 0};
      case Rotation::UP_TO_DOWN:
      case Rotation::DOWN_TO_UP:
        return cell == handle || cell == handle + Vec2{0, 1};
    }
  }

  [[nodiscard]] constexpr std::array<Vec2, 3> downstream_ingress_cells() const {
    return ::profit::downstream_ingress_cells(egress(), rotation);
  }

  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    switch (rotation) {
      case Rotation::LEFT_TO_RIGHT:
      case Rotation::RIGHT_TO_LEFT:
        return {handle + Vec2{-1, 0}, handle + Vec2{0, 0}, handle + Vec2{1, 0},
                handle + Vec2{2, 0}};
      case Rotation::UP_TO_DOWN:
      case Rotation::DOWN_TO_UP:
        return {handle + Vec2{0, -1}, handle + Vec2{0, 0}, handle + Vec2{0, 1},
                handle + Vec2{0, 2}};
    }
  }
};

struct Factory {
  constexpr static Vec2 DIMENSIONS{5, 5};
  constexpr static size_t OCCUPIED_CELL_COUNT =
      static_cast<size_t>(DIMENSIONS.x()) * static_cast<size_t>(DIMENSIONS.y());
  Vec2 handle;
  FactoryType type{};
  bool operator==(const Factory& other) const = default;

  // todo: kürzer
  [[nodiscard]] constexpr std::array<Vec2, OCCUPIED_CELL_COUNT> occupied_cells() const {
    return {handle + Vec2{0, 0}, handle + Vec2{1, 0}, handle + Vec2{2, 0}, handle + Vec2{3, 0},
            handle + Vec2{4, 0}, handle + Vec2{0, 1}, handle + Vec2{1, 1}, handle + Vec2{2, 1},
            handle + Vec2{3, 1}, handle + Vec2{4, 1}, handle + Vec2{0, 2}, handle + Vec2{1, 2},
            handle + Vec2{2, 2}, handle + Vec2{3, 2}, handle + Vec2{4, 2}, handle + Vec2{0, 3},
            handle + Vec2{1, 3}, handle + Vec2{2, 3}, handle + Vec2{3, 3}, handle + Vec2{4, 3},
            handle + Vec2{0, 4}, handle + Vec2{1, 4}, handle + Vec2{2, 4}, handle + Vec2{3, 4},
            handle + Vec2{4, 4}};
  }
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
          return coordinate + Vec2{1, -1};
        case Rotation::UP_TO_DOWN:
          return coordinate + Vec2{0, 1};
        case Rotation::RIGHT_TO_LEFT:
          return coordinate + Vec2{-2, 0};
        case Rotation::DOWN_TO_UP:
          return coordinate + Vec2{-1, -2};
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
    return ::profit::downstream_ingress_cells(egress(), rotation);
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
