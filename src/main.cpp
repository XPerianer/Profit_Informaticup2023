#include <iostream>
#include <nlohmann/json.hpp>

struct Input {
  int32_t width;
  int32_t height;
  int32_t turns;
  int32_t time;

  static constexpr int32_t DEFAULT_TIME = 300;

  // TODO: objects
  // TODO: products
};

Input parse_input() {
  nlohmann::json json_input;
  std::cin >> json_input;

  Input input{};
  input.width = json_input["width"];
  input.height = json_input["height"];
  input.turns = json_input["turns"];
  input.time = json_input.value("time", Input::DEFAULT_TIME);

  return input;
}

int main() {
  Input input = parse_input();

  std::cout << "We have " << input.time << " seconds to do something smart" << std::endl;
}
