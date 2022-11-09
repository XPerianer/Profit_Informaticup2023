#pragma once
#include "ObjectInterface.cpp"

class Factory : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  std::string type() override { return "factory"; }
  char toChar() override { return 'f'; }

  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};