#pragma once

#include "geometry/two_dimensional_vector.hpp"
#include "geometry/vector.hpp"
#include "utils.hpp"

/*
 * A helper class that allows us to look up information per cell on a play field.
 * To simplify our algorithms, we allow access out-of-bounds and always return a specific value
 * in these cases. This allows us to not care about field borders in algorithms
 */
template <typename CellT, CellT OutOfBoundsValue, CellT InitialValue = CellT{}>
class Field {
 public:
  explicit Field(geometry::Vector dimensions) : dimensions_{dimensions}, map_(dimensions) {}

  [[nodiscard]] CellT at(geometry::Vector coordinates) const {
    if (coordinates.x() < 0 || coordinates.y() < 0 || coordinates.x() >= dimensions_.width() ||
        coordinates.y() >= dimensions_.height()) {
      return OutOfBoundsValue;
    }
    return map_[coordinates];
  }

  void set(geometry::Vector coordinates, CellT value) {
    DEBUG_ASSERT(coordinates.x() >= 0 && coordinates.y() >= 0 &&
                     coordinates.x() < dimensions_.width() &&
                     coordinates.y() < dimensions_.height(),
                 "Out-of-bounds write on field");
    map_[coordinates] = value;
  }

  [[nodiscard]] geometry::Vector dimensions() const { return dimensions_; }

 private:
  geometry::Vector dimensions_;
  geometry::TwoDimensionalVector<CellT, InitialValue> map_;
};
