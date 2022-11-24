#pragma once
#include <vector>

#include "utils.hpp"

template <typename T>
class TwoDimensionalVector {
 public:
  explicit TwoDimensionalVector(Vec2 dimensions)
      : data_(dimensions.width() * dimensions.height()), width_{dimensions.width()} {}

  T& operator[](Vec2 index) { return data_[index.y() * width_ + index.y()]; }

 private:
  std::vector<T> data_;
  size_t width_;
};

template <size_t OffsetX, size_t OffsetY, typename ContainerT>
class OffsetAccess {
 public:
  explicit OffsetAccess(ContainerT container) : data_{container} {};
  auto& operator[](Vec2 index) { return data_[{index.x() - OffsetX, index.y() - OffsetY}]; }

 private:
  ContainerT data_;
};
