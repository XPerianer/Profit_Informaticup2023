#pragma once
#include <concepts>
#include <vector>

#include "geometry/vec2.hpp"
#include "miscellany/assert.hpp"

namespace geometry {

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
    explicit constexpr Iterator(const Rectangle& rectangle)
        : current_value_(rectangle.top_left()), rectangle_{&rectangle} {}

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
  /**
   * Extents are defined in a half-open manner [top_left, bottom_right[, following
   * https://www.cs.utexas.edu/users/EWD/transcriptions/EWD08xx/EWD831.html.
   * Use inner_bottom_right instead of bottom_right for a closed-interval.
   */
  [[nodiscard]] constexpr Vec2 bottom_right() const { return bottom_right_; }
  [[nodiscard]] constexpr Vec2 inner_top_left() const { return top_left_; }
  [[nodiscard]] constexpr Vec2 inner_bottom_right() const { return bottom_right_ - Vec2{1, 1}; }
  [[nodiscard]] constexpr Vec2 dimensions() const { return dimensions_; }

  [[nodiscard]] constexpr static Rectangle from_top_left_and_dimensions(Vec2 top_left,
                                                                        Vec2 dimensions) {
    return {top_left, dimensions};
  }

  [[nodiscard]] constexpr static Rectangle from_top_left_and_bottom_right(Vec2 top_left,
                                                                          Vec2 bottom_right) {
    return {top_left, bottom_right - top_left};
  }

 private:
  constexpr Rectangle(Vec2 top_left, Vec2 dimensions)
      : top_left_{top_left}, bottom_right_{top_left + dimensions}, dimensions_{dimensions} {
    DEBUG_ASSERT(bottom_right_.x() > top_left_.x() && bottom_right_.y() > top_left_.y(),
                 "Zero area rectangle not allowed.");
  }

  Vec2 top_left_;
  Vec2 bottom_right_;
  Vec2 dimensions_;
};

[[nodiscard]] constexpr bool is_on_border(const Rectangle& rect, Vec2 coordinate) {
  return ((coordinate.x() == rect.top_left().x() ||
           coordinate.x() == rect.inner_bottom_right().x()) &&
          coordinate.y() >= rect.top_left().y() && coordinate.y() < rect.bottom_right().y()) ||
         ((coordinate.y() == rect.top_left().y() ||
           coordinate.y() == rect.inner_bottom_right().y()) &&
          coordinate.x() >= rect.top_left().x() && coordinate.x() < rect.bottom_right().x());
}

/* Cells that are connected to a border cell of the rectangle */
[[nodiscard]] constexpr std::vector<Vec2> outer_connected_border_cells(const Rectangle& rect) {
  std::vector<Vec2> result(2 * static_cast<int>(rect.dimensions().width()) +
                           2 * static_cast<int>(rect.dimensions().height()));

  auto it = result.begin();

  for (Coordinate offset = 0; offset < rect.dimensions().width(); ++offset) {
    *it++ = rect.top_left() + Vec2{offset, -1};
    *it++ = rect.top_left() + Vec2{offset, rect.dimensions().height()};
  }

  for (Coordinate offset = 0; offset < rect.dimensions().height(); ++offset) {
    *it++ = rect.top_left() + Vec2{-1, offset};
    *it++ = rect.top_left() + Vec2{rect.dimensions().width(), offset};
  }

  return result;
}

[[nodiscard]] constexpr Rectangle::Iterator begin(const Rectangle& rect) {
  return Rectangle::Iterator(rect);
}
[[nodiscard]] constexpr Rectangle::Iterator end(const Rectangle& /*rect*/) { return {}; }

}  // namespace geometry
