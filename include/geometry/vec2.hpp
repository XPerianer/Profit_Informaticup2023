#pragma once
#include <iostream>

#include "geometry/coordinate.hpp"

namespace geometry {

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

  friend std::ostream& operator<<(std::ostream& stream, const Vec2& vec2);

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

inline constexpr Coordinate manhattan_distance(const Vec2& lhs, const Vec2& rhs) {
  return std::abs(static_cast<Coordinate::UnderlyingT>(lhs.x() - rhs.x())) +
         std::abs(static_cast<Coordinate::UnderlyingT>(lhs.y() - rhs.y()));
}

inline std::ostream& operator<<(std::ostream& stream, const Vec2& vec2) {
  stream << "(" << static_cast<int>(vec2.x()) << ", " << static_cast<int>(vec2.y()) << ")";
  return stream;
}

}  // namespace geometry
