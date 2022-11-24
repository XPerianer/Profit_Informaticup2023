#pragma once
#include <concepts>

#include "vec2.hpp"

namespace utils {

template <typename T>
concept ThingWithHandleAndDimensions = requires(T thing) {
  { thing.handle } -> std::convertible_to<Vec2>;
  { thing.dimensions } -> std::convertible_to<Vec2>;
};

template <ThingWithHandleAndDimensions ThingT>
Vec2 bottom_right(ThingT thing) {
  return {thing.handle.x() + thing.dimensions.width(),
          thing.handle.y() + thing.dimensions.height()};
}

inline bool is_border(Vec2 coordinate, Vec2 top_left, Vec2 bottom_right) {
  return coordinate.x() == top_left.x() || coordinate.x() == bottom_right.x() - 1 ||
         coordinate.y() == top_left.y() || coordinate.y() == bottom_right.y() - 1;
}

template <class... Ts>
struct overloaded : Ts... {  // NOLINT(fuchsia-multiple-inheritance)
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace utils
