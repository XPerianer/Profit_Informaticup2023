#pragma once
#include <concepts>

#include "geometry/vector.hpp"

namespace geometry {

template <typename T>
concept ThingWithHandleAndDimensions = requires(T thing) {
  { thing.handle } -> std::convertible_to<Vector>;
  { thing.dimensions } -> std::convertible_to<Vector>;
};

class Rectangle {
 public:
  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Vector;
    using pointer = const Vector*;
    using reference = const Vector&;

    Iterator() = default;
    explicit Iterator(const Rectangle& rectangle) : rectangle_{&rectangle} {}

    reference operator*() const { return current_value_; }
    pointer operator->() const { return &current_value_; }

    Iterator& operator++() {
      current_value_ += {1, 0};

      if (current_value_.x() == rectangle_->bottom_right().x()) {
        current_value_ = {rectangle_->top_left().x(), current_value_.y() + 1};
        if (current_value_.y() == rectangle_->bottom_right().y()) {
          current_value_ = {};
          rectangle_ = nullptr;
        }
      }

      return *this;
    }

    Iterator operator++(int) {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    auto operator<=>(const Iterator& other) const = default;

   private:
    Vector current_value_{};
    const Rectangle* rectangle_ = nullptr;
  };

  [[nodiscard]] Vector top_left() const { return top_left_; }
  [[nodiscard]] Vector bottom_right() const { return bottom_right_; }
  [[nodiscard]] Vector dimensions() const { return dimensions_; }

  static Rectangle from_top_left_and_dimensions(Vector top_left, Vector dimensions) {
    return {top_left, dimensions};
  }

  static Rectangle from_top_left_and_bottom_right(Vector top_left, Vector bottom_right) {
    return {top_left, bottom_right - top_left};
  }

 private:
  Rectangle(Vector top_left, Vector dimensions)
      : top_left_{top_left}, bottom_right_{top_left + dimensions}, dimensions_{dimensions} {}

  Vector top_left_;
  Vector bottom_right_;
  Vector dimensions_;
};

inline bool is_on_border(const Rectangle& rect, Vector coordinate) {
  return coordinate.x() == rect.top_left().x() || coordinate.x() == rect.bottom_right().x() - 1 ||
         coordinate.y() == rect.top_left().y() || coordinate.y() == rect.bottom_right().y() - 1;
}

inline Rectangle left_border(const Rectangle& rect) {
  return Rectangle::from_top_left_and_dimensions(rect.top_left(), {1, rect.dimensions().height()});
}

[[nodiscard]] inline Rectangle::Iterator begin(const Rectangle& rect) {
  return Rectangle::Iterator(rect);
}
[[nodiscard]] inline Rectangle::Iterator end(const Rectangle& /*rect*/) { return {}; }

}  // namespace geometry
