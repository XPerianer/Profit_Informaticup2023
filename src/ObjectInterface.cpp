#pragma once
#include <optional>
#include <string>
#include <vector>

#include "Field.cpp"

class ObjectInterface {
 public:
  explicit ObjectInterface(uint8_t m_x, uint8_t m_y) : x(m_x), y(m_y) {}
  explicit ObjectInterface(uint8_t m_x, uint8_t m_y, uint8_t m_subtype)
      : x(m_x), y(m_y), subtype(m_subtype) {}

  uint8_t getX() { return x; }
  uint8_t getY() { return y; }
  std::optional<uint8_t> getSubtype() { return subtype; }

  virtual std::string type() = 0;
  virtual char toChar() = 0;

  virtual std::vector<std::tuple<uint8_t, uint8_t>> getOccupiedFields() = 0;
  virtual std::vector<std::tuple<uint8_t, uint8_t>> getEntrances() = 0;
  virtual std::vector<std::tuple<uint8_t, uint8_t>> getExits() = 0;

  friend std::ostream& operator<<(std::ostream& s, ObjectInterface& o) {
    s << o.toChar();
    return s;
  }

 protected:
  const uint8_t x;
  const uint8_t y;
  const std::optional<uint8_t> subtype;
};