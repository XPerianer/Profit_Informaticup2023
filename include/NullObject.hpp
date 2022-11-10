#pragma once

#include <string>
#include <vector>

#include "ObjectInterface.hpp"

class NullObject : public ObjectInterface {
 public:
  // TODO: Richard: Evaluieren
  NullObject() : ObjectInterface({0,0}) {}

  [[nodiscard]] std::string type() const override { return "null"; }
  [[nodiscard]] char toChar() const override { return ' '; }

  [[nodiscard]] std::vector<Vec2> occupiedFields() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> entrances() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> exits() const override { return {}; }
};