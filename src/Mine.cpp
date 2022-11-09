#pragma once
#include "ObjectInterface.cpp"

class Mine : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  std::string type() override { return "mine"; }
  char toChar() override { return 'm'; }
  
  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};