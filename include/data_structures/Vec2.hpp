#pragma once

class Vec2 {
 public:
  using CoordT = signed char;
  Vec2() = default;
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Vec2(CoordT arg1, CoordT arg2) : x_{arg1}, y_{arg2} {}

  [[nodiscard]] CoordT x() const { return x_; };
  [[nodiscard]] CoordT y() const { return y_; };

  [[nodiscard]] CoordT width() const { return x_; };
  [[nodiscard]] CoordT height() const { return y_; };

  bool operator==(const Vec2& other) const = default;

 private:
  CoordT x_;
  CoordT y_;
};