#pragma once

#include <span>

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

  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Vec2;
    using pointer = const Vec2*;
    using reference = const Vec2&;

    constexpr Iterator() = default;
    explicit constexpr Iterator(const Field& field) : field_{&field} {}

    constexpr reference operator*() const { return current_value_; }
    constexpr pointer operator->() const { return &current_value_; }

    constexpr Iterator& operator++() {
      current_value_ += {1, 0};

      if (current_value_.x() == field_->dimensions().x()) {
        current_value_ = {0, current_value_.y() + 1};
        if (current_value_.y() == field_->dimensions().y()) {
          current_value_ = {};
          field_ = nullptr;
        }
      }

      return *this;
    }

    constexpr Iterator operator++(int) {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr auto operator<=>(const Iterator& other) const = default;

   private:
    Vec2 current_value_{0, 0};
    const Field* field_ = nullptr;
  };

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
  [[nodiscard]] std::span<const CellT> map() { return map_.span(); }

  [[nodiscard]] constexpr friend Iterator begin(const Field& field) { return Iterator(field); }
  [[nodiscard]] constexpr friend Iterator end(const Field& /*field*/) { return {}; }

 private:
  geometry::Vec2 dimensions_;
  geometry::TwoDimensionalVector<CellT, InitialValue> map_;
};

template <typename FieldT>
bool any_neighbor_is(const FieldT& field, Vec2 cell, const typename FieldT::CellT& cell_value) {
  bool any_is = false;
  any_is |= field.at(cell + Vec2{0, -1}) == cell_value;
  any_is |= field.at(cell + Vec2{-1, 0}) == cell_value;
  any_is |= field.at(cell + Vec2{1, 0}) == cell_value;
  any_is |= field.at(cell + Vec2{0, 1}) == cell_value;
  return any_is;
}

}  // namespace profit
