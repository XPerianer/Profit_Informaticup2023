#pragma once
#include <compare>
#include <cstdint>
#include <limits>

#include "assert.hpp"

struct CoordT {
  using UnderlyingT = int8_t;

  CoordT() = default;

  template <std::integral T>
  CoordT(T value)  // NOLINT(google-explicit-constructor): We want instances to be implicitly
                   // constructible to allow arithmetic with integer literals.
      : value_{static_cast<UnderlyingT>(value)} {
    DEBUG_ASSERT(value >= std::numeric_limits<CoordT::UnderlyingT>::min(), "Coordinate overflow");
    DEBUG_ASSERT(value <= std::numeric_limits<CoordT::UnderlyingT>::max(), "Coordinate overflow");
  }

  template <std::integral T>
  explicit operator T() const {
    return value_;
  }

  auto operator<=>(const CoordT& other) const = default;

  CoordT& operator++() {
    ++value_;
    return *this;
  }

  CoordT& operator--() {
    --value_;
    return *this;
  }

 private:
  UnderlyingT value_ = 0;
};

inline CoordT operator+(CoordT lhs, CoordT rhs) {
  return CoordT{static_cast<CoordT::UnderlyingT>(lhs) + static_cast<CoordT::UnderlyingT>(rhs)};
}

inline CoordT operator-(CoordT lhs, CoordT rhs) {
  return CoordT{static_cast<CoordT::UnderlyingT>(lhs) - static_cast<CoordT::UnderlyingT>(rhs)};
}

class Vec2 {
 public:
  Vec2() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters): Order for x and y is pretty clear
  Vec2(CoordT arg1, CoordT arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] CoordT x() const { return x_; };
  [[nodiscard]] CoordT y() const { return y_; };

  [[nodiscard]] CoordT width() const { return x_; };
  [[nodiscard]] CoordT height() const { return y_; };

  auto operator<=>(const Vec2& other) const = default;

  Vec2& operator+=(const Vec2& other) {
    x_ = x_ + other.x_;
    y_ = y_ + other.y_;
    return *this;
  }

  Vec2& operator-=(const Vec2& other) {
    x_ = x_ - other.x_;
    y_ = y_ - other.y_;
    return *this;
  }

 private:
  CoordT x_;
  CoordT y_;
};

inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
  Vec2 result{lhs};
  result += rhs;
  return result;
}

inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
  Vec2 result{lhs};
  result -= rhs;
  return result;
}

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
    Vec2 current_value_{};
    const Rectangle* rectangle_ = nullptr;
  };

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters): We'll have to live with that
  Rectangle(Vec2 top_left, Vec2 dimensions)
      : top_left_{top_left}, bottom_right_{top_left + dimensions}, dimensions_{dimensions} {}

  template <ThingWithHandleAndDimensions ThingT>
  static Rectangle from(ThingT thing) {
    // TODO: This is a bit wrong: handle might not be top-left (e.g. mine). Maybe use more
    // expressive accessor here (e.g. `top_left()`)?
    return Rectangle(thing.handle, thing.dimensions);
  }

  [[nodiscard]] Vec2 top_left() const { return top_left_; }
  [[nodiscard]] Vec2 bottom_right() const { return bottom_right_; }
  [[nodiscard]] Vec2 dimensions() const { return dimensions_; }

 private:
  Vec2 top_left_;
  Vec2 bottom_right_;
  Vec2 dimensions_;
};

inline bool is_on_border(const Rectangle& rect, Vec2 coordinate) {
  return coordinate.x() == rect.top_left().x() || coordinate.x() == rect.bottom_right().x() - 1 ||
         coordinate.y() == rect.top_left().y() || coordinate.y() == rect.bottom_right().y() - 1;
}

inline Rectangle left_border(const Rectangle& rect) {
  return Rectangle{rect.top_left(), {1, rect.dimensions().height()}};
}

[[nodiscard]] inline Rectangle::Iterator begin(const Rectangle& rect) {
  return Rectangle::Iterator(rect);
}
[[nodiscard]] inline Rectangle::Iterator end(const Rectangle& /*rect*/) { return {}; }
