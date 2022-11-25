#pragma once
#include <concepts>
#include <cstdint>
#include <limits>

#include "assert.hpp"

namespace geometry {

struct Coordinate {
  using UnderlyingT = int8_t;

  Coordinate() = default;

  template <std::integral T>
  Coordinate(T value)  // NOLINT(google-explicit-constructor): We want instances to be implicitly
                       // constructible to allow arithmetic with integer literals.
      : value_{static_cast<UnderlyingT>(value)} {
    DEBUG_ASSERT(value >= std::numeric_limits<Coordinate::UnderlyingT>::min(),
                 "Coordinate overflow");
    DEBUG_ASSERT(value <= std::numeric_limits<Coordinate::UnderlyingT>::max(),
                 "Coordinate overflow");
  }

  template <std::integral T>
  explicit operator T() const {
    return value_;
  }

  auto operator<=>(const Coordinate& other) const = default;

  Coordinate& operator++() {
    ++value_;
    return *this;
  }

  Coordinate& operator--() {
    --value_;
    return *this;
  }

 private:
  UnderlyingT value_ = 0;
};

inline Coordinate operator+(Coordinate lhs, Coordinate rhs) {
  return Coordinate{static_cast<Coordinate::UnderlyingT>(lhs) +
                    static_cast<Coordinate::UnderlyingT>(rhs)};
}

inline Coordinate operator-(Coordinate lhs, Coordinate rhs) {
  return Coordinate{static_cast<Coordinate::UnderlyingT>(lhs) -
                    static_cast<Coordinate::UnderlyingT>(rhs)};
}

}  // namespace geometry
