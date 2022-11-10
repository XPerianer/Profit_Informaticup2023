#pragma once
#include <string>
#include <vector>

#include "ObjectInterface.hpp"

class Deposit : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  [[nodiscard]] std::string type() const override { return "deposit"; }
  // TODO: Deposit resource type
  [[nodiscard]] char toChar() const override { return 'd'; }

  [[nodiscard]] std::vector<Vec2> occupiedFields() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> entrances() const override { return {}; }
  [[nodiscard]] std::vector<Vec2> exits() const override { return {}; }
};