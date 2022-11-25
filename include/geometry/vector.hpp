#pragma once
#include "geometry/coordinate.hpp"

namespace geometry {

// TODO: Naming: Collides with std::vector (especially when considering `TwoDimensionalVector`)
class Vector {
 public:
  Vector() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters): Order for x and y is pretty clear
  Vector(Coordinate arg1, Coordinate arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] Coordinate x() const { return x_; };
  [[nodiscard]] Coordinate y() const { return y_; };

  [[nodiscard]] Coordinate width() const { return x_; };
  [[nodiscard]] Coordinate height() const { return y_; };

  auto operator<=>(const Vector& other) const = default;

  Vector& operator+=(const Vector& other) {
    x_ = x_ + other.x_;
    y_ = y_ + other.y_;
    return *this;
  }

  Vector& operator-=(const Vector& other) {
    x_ = x_ - other.x_;
    y_ = y_ - other.y_;
    return *this;
  }

 private:
  Coordinate x_;
  Coordinate y_;
};

inline Vector operator+(const Vector& lhs, const Vector& rhs) {
  Vector result{lhs};
  result += rhs;
  return result;
}

inline Vector operator-(const Vector& lhs, const Vector& rhs) {
  Vector result{lhs};
  result -= rhs;
  return result;
}

}  // namespace geometry
