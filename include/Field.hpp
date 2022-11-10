#pragma once
#include <algorithm>
#include <iostream>
#include <limits>
#include <tuple>
#include <variant>
#include <vector>

#include "NullObject.hpp"
#include "ObjectInterface.hpp"
#include "assert.hpp"

using ObjectVariant =
    std::variant<NullObject>;

struct Field {
  using FieldIdT = uint32_t;

  constexpr static FieldIdT EMPTY = std::numeric_limits<FieldIdT>::max();
  inline static const ObjectVariant null_object = NullObject();

 public:
  explicit Field(Vec2 dimensions)
      : ids(static_cast<size_t>(dimensions.x) * dimensions.y, EMPTY),
        height_(dimensions.y),
        width_(dimensions.x) {}

  void insert(const std::vector<ObjectVariant>& toInsert) {
    if (std::any_of(toInsert.begin(), toInsert.end(),
                    [this](const auto& object) { return !this->place(object); })) {
      FAIL("Trying to initialize field with object out of bounds or overlapping");
    }
  }

  const ObjectVariant& operator[](Vec2 coordinate) const {
    DEBUG_ASSERT(coordinate.x < width_ && coordinate.y < height_, "Trying to access out of bounds");
    if (getId(coordinate) == EMPTY) {
      return null_object;
    }

    return objects[getId(coordinate)];
  }

  bool place(const ObjectVariant& object) {
    std::vector<Vec2> neededCoords =
        std::visit([](const auto& typed_object) { return typed_object.occupiedFields(); }, object);

    if (std::any_of(neededCoords.begin(), neededCoords.end(), [this](auto coord) {
          return coord.x >= width_ || coord.y >= height_ ||
                 (*this)[coord] == null_object;
        })) {
      return false;
    }

    ObjectVariant copy = object;

    objects.push_back(object);
    std::for_each(neededCoords.begin(), neededCoords.end(), [this](const auto & coord) {
      ids[width_ * coord.y + coord.x ] = objects.size();
    });
    return true;
  }

  [[nodiscard]] Vec2 dimensions() const { return {width_, height_}; }

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