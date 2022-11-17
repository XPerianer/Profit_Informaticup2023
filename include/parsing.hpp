#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

#include "assert.hpp"
#include "landscape.hpp"
#include "placeable.hpp"

namespace parsing {

struct Input {
  uint8_t width;
  uint8_t height;
  int32_t turns;
  int32_t time;
  std::vector<Product> products;
  std::vector<LandscapeObject> objects;

  static constexpr int32_t DEFAULT_TIME = 300;
};

inline LandscapeObject parse_object(const nlohmann::json& input) {
  std::string type = input["type"];
  Vec2 coordinate = {input["x"], input["y"]};
  Vec2 dimensions = {input["width"], input["height"]};

  if (type == "deposit") {
    return Deposit{coordinate, dimensions, input["subtype"]};
  }
  if (type == "obstacle") {
    return Obstacle{coordinate, dimensions};
  }

  FAIL("unexpected input object type.");
}

inline Input parse(std::istream& stream) {
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
    input.objects.push_back(parse_object(object_json));
  }

  input.time = json_input.value("time", Input::DEFAULT_TIME);

  return input;
}
};  // namespace parsing