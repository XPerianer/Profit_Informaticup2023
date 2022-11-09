#pragma once
#include <algorithm>
#include <iostream>
#include <tuple>
#include <variant>
#include <vector>

#include "NullObject.cpp"
#include "ObjectInterface.cpp"

template <typename T>
struct Field {
 public:
  Field(uint8_t width, uint8_t height)
      : ids(width * height, -1), objects({}), HEIGHT(height), WIDTH(width) {}

  Field(uint8_t width, uint8_t height, std::vector<Object> toInsert)
      : ids(width * height, -1), objects({}), HEIGHT(height), WIDTH(width) {
    if (std::any_of(toInsert.begin(), toInsert.end(),
                    [this](auto object) { return !this->place(object); })) {
      throw std::out_of_range(
          "Trying to initialize field with object out of bounds or overlapping");
    }
  }

  Object& operator()(const uint8_t x, const uint8_t y) {
    if (x >= WIDTH || y >= HEIGHT) throw std::out_of_range("Trying to access out of bounds.");
    if (getId(x, y) < 0) return NullObject(0, 0);

    return objects[getId(x, y)];
  }

  Object operator()(const uint8_t x, const uint8_t y) const {
    if (x >= WIDTH || y >= HEIGHT) throw std::out_of_range("Trying to access out of bounds.");
    if (getId(x, y) < 0) return NullObject(0, 0);

    return objects[getId(x, y)];
  }

  bool place(const Object& object) {
    std::vector<std::tuple<uint8_t, uint8_t>> neededCoords = object.getOccupiedFields();

    if (std::any_of(neededCoords.begin(), neededCoords.end(), [this](auto coord) {
          return std::get<0>(coord) >= WIDTH || std::get<1>(coord) >= HEIGHT ||
                 this(std::get<0>(coord), std::get<1>(coord)) >= 0;
        })) {
      return false;
    }

    objects.push_back(object);
    std::for_each(neededCoords.begin(), neededCoords.end(), [this](auto coord) {
      this(std::get<0>(coord), std::get<1>(coord), objects.size());
    });
    return true;
  }

  std::tuple<uint8_t, uint8_t> xy() const { return {WIDTH, HEIGHT}; }

  friend std::ostream& operator<<(std::ostream& s, const Field& f) {
    const auto [WIDTH, HEIGHT] = f.xy();
    for (auto y = 0; y < HEIGHT; y++) {
      for (auto x = 0; x < WIDTH; x++) {
        s << f(x, y) << ' ';
      }
      s << '\n';
    }

    return s;
  }

 private:
  T getId(const uint8_t x, const uint8_t y) const {
    if (x >= WIDTH || y >= HEIGHT) throw std::out_of_range("Trying to access out of bounds.");
    return ids[WIDTH * y + x];
  }

 protected:
  std::vector<T> ids;
  std::vector<Object> objects;
  const uint8_t WIDTH;
  const uint8_t HEIGHT;
};