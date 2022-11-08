#include <iostream>
#include <tuple>
#include <vector>

struct Field {
 public:
  Field(std::size_t width, std::size_t height)
      : contents(width * height, -1), HEIGHT(height), WIDTH(width) {}

  int32_t& operator()(std::size_t x, std::size_t y) {
    if (x >= WIDTH || y >= HEIGHT) throw std::out_of_range("Trying to access out of bounds.");
    return contents[WIDTH * y + x];
  }

  int32_t operator()(std::size_t x, std::size_t y) const {
    if (x >= WIDTH || y >= HEIGHT) throw std::out_of_range("Trying to access out of bounds.");
    return contents[WIDTH * y + x];
  }

  auto& underlying_array() { return contents; }

  std::tuple<std::size_t, std::size_t> dimensions() const { return {WIDTH, HEIGHT}; }

  friend std::ostream& operator<<(std::ostream& s, const Field& f) {
    const auto [WIDTH, HEIGHT] = f.dimensions();
    for (auto y = 0; y < HEIGHT; y++) {
      for (auto x = 0; x < WIDTH; x++) {
        s << f(x, y) << ' ';
      }
      s << '\n';
    }

    return s;
  }

 protected:
  std::vector<int32_t> contents;
  const size_t WIDTH;
  const size_t HEIGHT;
};