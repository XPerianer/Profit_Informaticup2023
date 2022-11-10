#pragma once
#include "ObjectInterface.hpp"

class Obstacle : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  [[nodiscard]] std::string type() const override { return "obstacle"; }
  [[nodiscard]] char toChar() const override { return 'x'; }

  [[nodiscard]] std::vector<Vec2> occupiedFields() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> entrances() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> exits() const override { return {}; }
};