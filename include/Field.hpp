#pragma once
#include <algorithm>
#include <iostream>
#include <limits>
#include <tuple>
#include <variant>
#include <vector>

#include "Combiner.hpp"
#include "Conveyor.hpp"
#include "Deposit.hpp"
#include "Factory.hpp"
#include "Mine.hpp"
#include "NullObject.hpp"
#include "ObjectInterface.hpp"
#include "Obstacle.hpp"
#include "assert.hpp"

using ObjectVariant =
    std::variant<Combiner, Conveyor, Deposit, Factory, Mine, NullObject, Obstacle>;

struct Field {
  using FieldIdT = uint32_t;

  constexpr static FieldIdT EMPTY = std::numeric_limits<FieldIdT>::max();

 public:
  explicit Field(Vec2 dimensions)
      : ids(static_cast<size_t>(dimensions.x) * dimensions.y, -1),
        objects({}),
        height_(dimensions.y),
        width_(dimensions.x) {}

  void insert(const std::vector<ObjectVariant>& toInsert) {
    if (std::any_of(toInsert.begin(), toInsert.end(),
                    [this](const auto& object) { return !this->place(object); })) {
      FAIL("Trying to initialize field with object out of bounds or overlapping");
    }
  }

  const ObjectVariant& operator[](Vec2 coordinate) const {
    static ObjectVariant null_object = NullObject();

    DEBUG_ASSERT(coordinate.x < width_ && coordinate.y < height_, "Trying to access out of bounds");
    if (getId(coordinate) == EMPTY) {
      return null_object;
    }

    return objects[getId(coordinate)];
  }

  bool place(const ObjectVariant& object) {
    std::vector<Vec2> neededCoords =
        std::visit([](const auto& typed_object) { return typed_object.occupiedFields(); });

    if (std::any_of(neededCoords.begin(), neededCoords.end(), [this](auto coord) {
          return std::get<0>(coord) >= width_ || std::get<1>(coord) >= height_ ||
                 this(std::get<0>(coord), std::get<1>(coord)) >= 0;
        })) {
      return false;
    }

    objects.push_back(object);
    std::for_each(neededCoords.begin(), neededCoords.end(), [this](auto coord) {
      this(std::get<0>(coord), std::get<1>(coord), objects.size());
    });
    return true;
  }

  [[nodiscard]] Vec2 dimensions() const { return {width_, height_}; }

  friend std::ostream& operator<<(std::ostream& s, const Field& f) {
    const auto dims = f.dimensions();
    for (uint8_t y = 0; y < dims.y; y++) {
      for (uint8_t x = 0; x < dims.x; x++) {
        s << std::visit([&](const auto& typed_object) -> char {return typed_object[{x, y}];}, f) << ' ';
      }
      s << '\n';
    }

    return s;
  }

 private:
  [[nodiscard]] FieldIdT getId(Vec2 coordinate) const {
    DEBUG_ASSERT(coordinate.x < width_ && coordinate.y < height_, "Trying to access out of bounds");
    return ids[width_ * coordinate.y + coordinate.x];
  }

  std::vector<FieldIdT> ids;
  std::vector<ObjectVariant> objects;
  uint8_t width_;
  uint8_t height_;
};