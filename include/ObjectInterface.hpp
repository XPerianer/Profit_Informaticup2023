#pragma once
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

enum Rotation {
  LEFT_TO_RIGHT = 0,
  UP_TO_DOWN = 1,
  RIGHT_TO_LEFT = 2,
  DOWN_TO_UP = 3,
};

struct Vec2 {
  uint8_t x;
  uint8_t y;
};

class ObjectInterface {
 public:
  explicit ObjectInterface(Vec2 coordinate, Rotation rotation = Rotation::LEFT_TO_RIGHT)
      : coordinate_(coordinate), rotation_(rotation) {}

  virtual ~ObjectInterface() = default;
  ObjectInterface(const ObjectInterface& other) = delete;
  ObjectInterface(ObjectInterface&& other) = default;
  ObjectInterface& operator=(const ObjectInterface& other) = delete;
  ObjectInterface& operator=(ObjectInterface&& other) = default;

  [[nodiscard]] Vec2 coordinate() const { return coordinate_; }
  Rotation rotation() { return rotation_; }

  // TODO(Richard): Evaluieren
  [[nodiscard]] virtual std::string type() const = 0;
  [[nodiscard]] virtual char toChar() const = 0;

  [[nodiscard]] virtual std::vector<Vec2> occupiedFields() const = 0;
  [[nodiscard]] virtual std::vector<Vec2> entrances() const = 0;
  [[nodiscard]] virtual std::vector<Vec2> exits() const = 0;

  friend std::ostream& operator<<(std::ostream& stream, const ObjectInterface& object) {
    stream << object.toChar();
    return stream;
  }

 private:
  Vec2 coordinate_;
  Rotation rotation_;
};