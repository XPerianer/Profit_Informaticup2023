#pragma once
#include <vector>

#include "geometry/vector.hpp"
#include "utils.hpp"

namespace geometry {

template <typename T, T InitialValue = T{}>
class TwoDimensionalVector {
 public:
  explicit TwoDimensionalVector(Vector dimensions)
      : data_(static_cast<size_t>(dimensions.width()) * static_cast<size_t>(dimensions.height()),
              InitialValue),
        dimensions_{dimensions} {}

  [[nodiscard]] Vector dimensions() const { return dimensions_; }

  T& operator[](Vector index) {
    return data_[static_cast<size_t>(index.y()) * static_cast<size_t>(dimensions_.width()) +
                 static_cast<size_t>(index.y())];
  }

 private:
  std::vector<T> data_;
  Vector dimensions_;
};

}  // namespace geometry
