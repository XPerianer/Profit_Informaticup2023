#pragma once
#include "ObjectInterface.cpp"

class Obstacle : public ObjectInterface {
 public:
  using ObjectInterface::ObjectInterface;
  Obstacle(int32_t m_x, int32_t m_y, int8_t m_subtype) = delete;

  std::string type() override { return "obstacle"; }
  char toChar() override { return 'x'; }

  std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() { return {}; }
  std::vector<std::tuple<uint8_t, uint8_t>> getExits() { return {}; }
};