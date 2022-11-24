#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "assert.hpp"
#include "resource_type.hpp"

class Requirements {
 public:
  Requirements() = default;

  Requirements(std::initializer_list<int> resource_counts)
      : Requirements(std::vector<int>(resource_counts)) {}

  explicit Requirements(std::vector<int> resource_counts) {
    ASSERT(resource_counts.size() == RESOURCE_TYPE_COUNT, "Invalid count of resources");
    std::copy(resource_counts.begin(), resource_counts.end(), requirements_.begin());
  }

  int32_t& operator[](ResourceType type) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): Enum value is in bounds.
    return requirements_[static_cast<size_t>(type)];
  }

  const int32_t& operator[](ResourceType type) const {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index): Enum value is in bounds.
    return requirements_[static_cast<size_t>(type)];
  }

  [[nodiscard]] const std::array<int32_t, RESOURCE_TYPE_COUNT>& as_array() const {
    return requirements_;
  }

  auto operator<=>(const Requirements& other) const = default;

 private:
  std::array<int32_t, RESOURCE_TYPE_COUNT> requirements_ = {};
};

using ProductType = Subtype;

struct Product {
  ProductType type{};
  Requirements requirements{};
  int32_t points{};
  bool operator==(const Product& other) const = default;
};
