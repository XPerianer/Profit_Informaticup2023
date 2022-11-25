#pragma once
#include <vector>

#include "data_structures/vec2.hpp"

template <typename T>
class TwoDimensionalVector {
 public:
  TwoDimensionalVector(size_t width, size_t height) : data_(width * height), width_{width} {}

  T& operator[](Vec2 index) { return data_[index.y() * width_ + index.y()]; }

 private:
  std::vector<T> data_;
  size_t width_;
};
