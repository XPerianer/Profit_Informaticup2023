#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include <variant>
#include <vector>

#include "Combiner.cpp"
#include "Conveyor.cpp"
#include "Deposit.cpp"
#include "Factory.cpp"
#include "Mine.cpp"
#include "Obstacle.cpp"

using Object = std::variant<Deposit, Obstacle, Mine, Conveyor, Combiner, Factory>;

struct Product {
  int8_t subtype;
  std::vector<int32_t> requirements;
  int32_t points;
};

struct Input {
  int32_t width;
  int32_t height;
  int32_t turns;
  int32_t time;
  std::vector<Product> products;
  std::vector<Object> objects;

  static constexpr int32_t DEFAULT_TIME = 300;
};

class InputParser {
 public:
  static Input parse(std::istream& is) {
    nlohmann::json json_input;
    is >> json_input;

    Input input{};
    input.width = json_input["width"];
    input.height = json_input["height"];
    input.turns = json_input["turns"];
    for (const auto& product_json : json_input["products"]) {
      input.products.push_back(
          {product_json["subtype"], product_json["resources"], product_json["points"]});
    }
    for (const auto& object_json : json_input["objects"]) {
      input.objects.push_back(parseObject(object_json));
    }

    input.time = json_input.value("time", Input::DEFAULT_TIME);

    return input;
  }

  static Object parseObject(const nlohmann::json& input) {
    std::string type = input["type"];
    int32_t x = input["x"];
    int32_t y = input["y"];
    if (type == "deposit") {
      return Deposit(x, y, input["subtype"]);
    } else if (type == "obstacle") {
      return Obstacle(x, y);
    } else if (type == "mine") {
      return Mine(x, y, input["subtype"]);
    } else if (type == "conveyor") {
      return Conveyor(x, y, input["subtype"]);
    } else if (type == "combiner") {
      return Combiner(x, y, input["subtype"]);
    } else {
      return Factory(x, y, input["subtype"]);
    }
  }
};