#pragma once
#include "ObjectInterface.cpp"

class Conveyor : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;

  std::string type() override { return "conveyor"; }
  char toChar() override { return '/'; }

  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};