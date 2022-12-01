#pragma once
#include <concepts>

#include "geometry/vec2.hpp"

namespace geometry {

template <typename T>
concept ThingWithHandleAndDimensions = requires(T thing) {
  { thing.handle } -> std::convertible_to<Vec2>;
  { thing.dimensions } -> std::convertible_to<Vec2>;
};

class Rectangle {
 public:
  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = Vec2;
    using pointer = const Vec2*;
    using reference = const Vec2&;

    constexpr Iterator() = default;
    explicit constexpr Iterator(const Rectangle& rectangle) : rectangle_{&rectangle} {}

    constexpr reference operator*() const { return current_value_; }
    constexpr pointer operator->() const { return &current_value_; }

    constexpr Iterator& operator++() {
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

    constexpr Iterator operator++(int) {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr auto operator<=>(const Iterator& other) const = default;

   private:
    Vec2 current_value_{};
    const Rectangle* rectangle_ = nullptr;
  };

  [[nodiscard]] constexpr Vec2 top_left() const { return top_left_; }
  [[nodiscard]] constexpr Vec2 bottom_right() const { return bottom_right_; }
  [[nodiscard]] constexpr Vec2 dimensions() const { return dimensions_; }

  constexpr static Rectangle from_top_left_and_dimensions(Vec2 top_left, Vec2 dimensions) {
    return {top_left, dimensions};
  }

  constexpr static Rectangle from_top_left_and_bottom_right(Vec2 top_left, Vec2 bottom_right) {
    return {top_left, bottom_right - top_left};
  }

 private:
  constexpr Rectangle(Vec2 top_left, Vec2 dimensions)
      : top_left_{top_left}, bottom_right_{top_left + dimensions}, dimensions_{dimensions} {}

  Vec2 top_left_;
  Vec2 bottom_right_;
  Vec2 dimensions_;
};

constexpr bool is_on_border(const Rectangle& rect, Vec2 coordinate) {
  return coordinate.x() == rect.top_left().x() || coordinate.x() == rect.bottom_right().x() - 1 ||
         coordinate.y() == rect.top_left().y() || coordinate.y() == rect.bottom_right().y() - 1;
}

constexpr Rectangle left_border(const Rectangle& rect) {
  return Rectangle::from_top_left_and_dimensions(rect.top_left(), {1, rect.dimensions().height()});
}

[[nodiscard]] constexpr Rectangle::Iterator begin(const Rectangle& rect) {
  return Rectangle::Iterator(rect);
}
[[nodiscard]] constexpr Rectangle::Iterator end(const Rectangle& /*rect*/) { return {}; }

}  // namespace geometry
