#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

enum Rotation {
  LEFT_TO_RIGHT = 0,
  UP_TO_DOWN = 1,
  RIGHT_TO_LEFT = 2,
  DOWN_TO_UP = 3,
};

class Vec2 {
 public:
  using CoordT = uint8_t;

  Vec2() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Vec2(CoordT arg1, CoordT arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] uint8_t x() const { return x_; };
  [[nodiscard]] uint8_t y() const { return y_; };

  [[nodiscard]] uint8_t width() const { return x_; };
  [[nodiscard]] uint8_t height() const { return y_; };

  bool operator==(const Vec2& other) const = default;

 private:
  CoordT x_;
  CoordT y_;
};

// TODO(Richard): Namespace / Code Structuring überlegen?
struct Product {
  static constexpr size_t resource_type_count = 8;

  int8_t subtype;
  std::array<int32_t, resource_type_count> requirements;
  int32_t points;
  bool operator==(const Product& other) const = default;
};
