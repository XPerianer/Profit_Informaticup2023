#pragma once

#include "geometry/two_dimensional_vector.hpp"
#include "geometry/vec2.hpp"
#include "utils.hpp"

namespace profit {

using geometry::Vec2;

/*
 * A helper class that allows us to look up information per cell on a play field.
 * To simplify our algorithms, we allow access out-of-bounds and always return a specific value
 * in these cases. This allows us to not care about field borders in algorithms
 */
template <typename CellT_, CellT_ OutOfBoundsValue, CellT_ InitialValue = CellT_{}>
class Field {
 public:
  using CellT = CellT_;

  explicit Field(geometry::Vec2 dimensions) : dimensions_{dimensions}, map_(dimensions) {}

  [[nodiscard]] CellT at(geometry::Vec2 coordinates) const {
    if (coordinates.x() < 0 || coordinates.y() < 0 || coordinates.x() >= dimensions_.width() ||
        coordinates.y() >= dimensions_.height()) {
      return OutOfBoundsValue;
    }
    return map_[coordinates];
  }

  void set(geometry::Vec2 coordinates, CellT value) {
    DEBUG_ASSERT(coordinates.x() >= 0 && coordinates.y() >= 0 &&
                     coordinates.x() < dimensions_.width() &&
                     coordinates.y() < dimensions_.height(),
                 "Out-of-bounds write on field");
    map_[coordinates] = value;
  }

  [[nodiscard]] geometry::Vec2 dimensions() const { return dimensions_; }

 private:
  geometry::Vec2 dimensions_;
  geometry::TwoDimensionalVector<CellT, InitialValue> map_;
};

template<typename FieldT>
bool any_neighbor_is(const FieldT& field, Vec2 cell, const typename FieldT::CellT& cell_value) {
  bool any_is = false;
  any_is |= field.at(cell + Vec2{0, -1}) == cell_value;
  any_is |= field.at(cell + Vec2{-1, 0}) == cell_value;
  any_is |= field.at(cell + Vec2{1, 0}) == cell_value;
  any_is |= field.at(cell + Vec2{0, 1}) == cell_value;
  return any_is;
}

}  // namespace profit
