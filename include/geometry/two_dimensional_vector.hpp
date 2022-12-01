#pragma once
#include <vector>

#include "geometry/vec2.hpp"
#include "utils.hpp"

namespace geometry {

template <typename T, T InitialValue = T{}>
class TwoDimensionalVector {
 public:
  explicit TwoDimensionalVector(Vec2 dimensions)
      : data_(static_cast<size_t>(dimensions.width()) * static_cast<size_t>(dimensions.height()),
              InitialValue),
        dimensions_{dimensions} {}

  [[nodiscard]] Vec2 dimensions() const { return dimensions_; }

  T& operator[](Vec2 index) {
    return data_[static_cast<size_t>(index.y()) * static_cast<size_t>(dimensions_.width()) +
                 static_cast<size_t>(index.y())];
  }

 private:
  std::vector<T> data_;
  Vec2 dimensions_;
};

}  // namespace geometry
