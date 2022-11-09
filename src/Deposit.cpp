#pragma once
#include "ObjectInterface.cpp"

class Deposit : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  std::string type() override { return "deposit"; }
  char toChar() override { return subtype.value(); }

  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};