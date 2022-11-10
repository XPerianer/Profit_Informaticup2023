#pragma once
#include "ObjectInterface.hpp"

class Mine : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  [[nodiscard]] std::string type() const override { return "mine"; }
  [[nodiscard]] char toChar() const override { return 'm'; }
  
  [[nodiscard]] std::vector<Vec2> occupiedFields() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> entrances() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> exits() const override { return {}; }
};