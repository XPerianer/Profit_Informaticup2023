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

  bool operator==(const Vec2& other) const = default;

 private:
  CoordT x_;
  CoordT y_;
};
