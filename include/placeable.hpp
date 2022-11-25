#pragma once

#include <variant>

#include "assert.hpp"
#include "geometry/vector.hpp"
#include "rotation.hpp"
#include "subtype.hpp"
#include "utils.hpp"

using geometry::Vector;

using FactoryType = Subtype;
constexpr size_t FACTORY_TYPE_COUNT = SUBTYPE_COUNT;

struct Combiner {
  Vector handle;
  Rotation rotation{};
  bool operator==(const Combiner& other) const = default;
};

struct Conveyor3 {
  Vector handle;
  Rotation rotation{};
  bool operator==(const Conveyor3& other) const = default;
};

struct Conveyor4 {
  Vector handle;
  Rotation rotation{};
  bool operator==(const Conveyor4& other) const = default;
};

struct Factory {
  Vector handle;
  FactoryType type{};
  bool operator==(const Factory& other) const = default;
};

struct Mine {
  Vector handle;
  Rotation rotation{};
  bool operator==(const Mine& other) const = default;

  static Mine with_ingress(Vector coordinate, Rotation rotation) {
    Vector handle;
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
