#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "assert.hpp"
#include "constants/subtype.hpp"

class Requirements {
 public:
  Requirements() = default;

  template <typename ContainerT>
  Requirements(const ContainerT& resource_counts) {
    ASSERT(resource_counts.size() == SUBTYPE_COUNT, "Invalid count of resources");
    std::copy(resource_counts.begin(), resource_counts.end(), requirements_.begin());
  }

  Requirements(std::initializer_list<int> resource_counts) {
    ASSERT(resource_counts.size() == SUBTYPE_COUNT, "Invalid count of resources");
    std::copy(resource_counts.begin(), resource_counts.end(), requirements_.begin());
  }

  int32_t& operator[](Subtype type) { return requirements_[static_cast<size_t>(type)]; }

  const int32_t& operator[](Subtype type) const { return requirements_[static_cast<size_t>(type)]; }

  [[nodiscard]] const std::array<int32_t, SUBTYPE_COUNT>& as_array() const { return requirements_; }

  auto operator<=>(const Requirements& other) const = default;

 private:
  std::array<int32_t, SUBTYPE_COUNT> requirements_ = {};
};

struct Product {
  Subtype type{};
  Requirements requirements{};
  int32_t points{};
  bool operator==(const Product& other) const = default;
};