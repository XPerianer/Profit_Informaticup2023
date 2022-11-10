#pragma once

#include <string>
#include <vector>

#include "ObjectInterface.hpp"

class NullObject {
 public:
  // TODO: Richard: Evaluieren
  NullObject() noexcept = default;

  bool operator==(const NullObject& other) const = default;

  [[nodiscard]] std::string type() const { return "null"; }
  [[nodiscard]] char toChar() const { return ' '; }

  [[nodiscard]] std::vector<Vec2> occupiedFields() const  { return {}; }
  [[nodiscard]] std::vector<Vec2> entrances() const  { return {}; }
  [[nodiscard]] std::vector<Vec2> exits() const  { return {}; }
};