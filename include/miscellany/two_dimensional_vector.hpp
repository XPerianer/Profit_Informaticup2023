#pragma once
#include <cstddef>
#include <span>
#include <vector>

#include "geometry/vec2.hpp"
#include "miscellany/utils.hpp"

namespace geometry {

template <typename T, T InitialValue = T{}>
class TwoDimensionalVector {
 public:
  explicit TwoDimensionalVector(Vec2 dimensions)
      : data_(static_cast<size_t>(dimensions.width()) * static_cast<size_t>(dimensions.height()),
              InitialValue),
        dimensions_{dimensions} {}

  [[nodiscard]] Vec2 dimensions() const { return dimensions_; }

  [[nodiscard]] std::span<T> span() { return std::span{data_}; }

  T& operator[](Vec2 index) { return data_[scalar_index(index)]; }

  const T& operator[](Vec2 index) const { return data_[scalar_index(index)]; }

 private:
  [[nodiscard]] size_t scalar_index(Vec2 index) const {
    return static_cast<size_t>(index.y()) * static_cast<size_t>(dimensions_.width()) +
           static_cast<size_t>(index.x());
  }

  std::vector<T> data_;
  Vec2 dimensions_;
};

}  // namespace geometry
