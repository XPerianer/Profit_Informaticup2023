#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

#include "Field.hpp"
#include "assert.hpp"

struct Product {
  int8_t subtype;
  std::vector<int32_t> requirements;
  int32_t points;
};

struct Input {
  uint8_t width;
  uint8_t height;
  int32_t turns;
  int32_t time;
  std::vector<Product> products;
  std::vector<ObjectVariant> objects;

  static constexpr int32_t DEFAULT_TIME = 300;
};

class InputParser {
 public:
  static Input parse(std::istream& stream) {
    nlohmann::json json_input;
    stream >> json_input;

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

  static Rotation subtype_as_rotation(int subtype) { return static_cast<Rotation>(subtype % 4); }

  static ObjectVariant parseObject(const nlohmann::json& input) {
    std::string type = input["type"];
    Vec2 coordinate = {input["x"], input["y"]};
    // if (type == "deposit") {
    //   return Deposit(coordinate, subtype_as_rotation(input["subtype"]));
    // }
    // if (type == "obstacle") {
    //   return Obstacle(coordinate);
    // }
    // if (type == "mine") {
    //   return Mine(coordinate, subtype_as_rotation(input["subtype"]));
    // }
    // if (type == "conveyor") {
    //   return Conveyor(coordinate, subtype_as_rotation(input["subtype"]));
    // }
    // if (type == "combiner") {
    //   return Combiner(coordinate, subtype_as_rotation(input["subtype"]));
    // }
    // if (type == "factory") {
    //   return Factory(coordinate, subtype_as_rotation(input["subtype"]));
    // }

    FAIL("unexpected input object type.");
  }
};