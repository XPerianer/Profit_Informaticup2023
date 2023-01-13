#pragma once
#include <concepts>
#include <cstdint>
#include <limits>

#include "miscellany/assert.hpp"

namespace geometry {

struct Coordinate {
  using UnderlyingT = int8_t;

  constexpr Coordinate() = default;

  // NOLINTNEXTLINE(google-explicit-constructor): We want implicit construction for arithmetic
  constexpr Coordinate(int value) : value_{static_cast<UnderlyingT>(value)} {
    DEBUG_ASSERT(value >= std::numeric_limits<Coordinate::UnderlyingT>::min(),
                 "Coordinate overflow");
    DEBUG_ASSERT(value <= std::numeric_limits<Coordinate::UnderlyingT>::max(),
                 "Coordinate overflow");
  }

  constexpr auto operator<=>(const Coordinate& other) const = default;

  template <std::integral T>
  explicit constexpr operator T() const {
    return value_;
  }

  constexpr Coordinate& operator++() {
    ++value_;
    return *this;
  }

  constexpr Coordinate& operator--() {
    --value_;
    return *this;
  }

 private:
  UnderlyingT value_ = 0;
};

constexpr Coordinate operator+(Coordinate lhs, Coordinate rhs) {
  return Coordinate{static_cast<Coordinate::UnderlyingT>(lhs) +
                    static_cast<Coordinate::UnderlyingT>(rhs)};
}

constexpr Coordinate operator-(Coordinate lhs, Coordinate rhs) {
  return Coordinate{static_cast<Coordinate::UnderlyingT>(lhs) -
                    static_cast<Coordinate::UnderlyingT>(rhs)};
}

}  // namespace geometry
