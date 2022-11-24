#pragma once
#include <compare>
#include <cstdint>
#include <limits>

#include "assert.hpp"

struct CoordT {
  using UnderlyingT = uint8_t;

  CoordT() = default;
  CoordT(UnderlyingT value) : value_{value} {}

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
  int result = static_cast<CoordT::UnderlyingT>(lhs) + static_cast<CoordT::UnderlyingT>(rhs);
  DEBUG_ASSERT(result >= std::numeric_limits<CoordT::UnderlyingT>::min(),
               "Coordinate arithmetic overflow");
  DEBUG_ASSERT(result <= std::numeric_limits<CoordT::UnderlyingT>::max(),
               "Coordinate arithmetic overflow");
  return static_cast<CoordT>(result);
}

inline CoordT operator-(CoordT lhs, CoordT rhs) {
  int result = static_cast<CoordT::UnderlyingT>(lhs) - static_cast<CoordT::UnderlyingT>(rhs);
  DEBUG_ASSERT(result >= std::numeric_limits<CoordT::UnderlyingT>::min(),
               "Coordinate arithmetic overflow");
  DEBUG_ASSERT(result <= std::numeric_limits<CoordT::UnderlyingT>::max(),
               "Coordinate arithmetic overflow");
  return static_cast<CoordT>(result);
}

class Vec2 {
 public:
  Vec2() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Vec2(CoordT arg1, CoordT arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] CoordT x() const { return x_; };
  [[nodiscard]] CoordT y() const { return y_; };

  [[nodiscard]] CoordT width() const { return x_; };
  [[nodiscard]] CoordT height() const { return y_; };

  bool operator==(const Vec2& other) const = default;

 private:
  CoordT x_;
  CoordT y_;
};
