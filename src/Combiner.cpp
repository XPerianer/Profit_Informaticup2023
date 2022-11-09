#pragma once
#include "ObjectInterface.cpp"

class Combiner : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  std::string type() override { return "combiner"; }
  char toChar() override { return 'c'; }

  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};