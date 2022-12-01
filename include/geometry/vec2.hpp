#pragma once
#include "geometry/coordinate.hpp"

namespace geometry {

// TODO: Naming: Collides with std::vector (especially when considering `TwoDimensionalVector`)
class Vec2 {
 public:
  constexpr Vec2() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters): Order for x and y is pretty clear
  constexpr Vec2(Coordinate arg1, Coordinate arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] constexpr Coordinate x() const { return x_; }
  [[nodiscard]] constexpr Coordinate y() const { return y_; }

  [[nodiscard]] constexpr Coordinate width() const { return x_; }
  [[nodiscard]] constexpr Coordinate height() const { return y_; }

  auto operator<=>(const Vec2& other) const = default;

  constexpr Vec2& operator+=(const Vec2& other) {
    x_ = x_ + other.x_;
    y_ = y_ + other.y_;
    return *this;
  }

  constexpr Vec2& operator-=(const Vec2& other) {
    x_ = x_ - other.x_;
    y_ = y_ - other.y_;
    return *this;
  }

 private:
  Coordinate x_;
  Coordinate y_;
};

inline constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
  Vec2 result{lhs};
  result += rhs;
  return result;
}

inline constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
  Vec2 result{lhs};
  result -= rhs;
  return result;
}

}  // namespace geometry
