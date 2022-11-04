#pragma once
#include <algorithm>
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
  Vec2 dimensions;
  int32_t turns;
  int32_t time;
  std::vector<Product> products;
  std::vector<LandscapeObject> objects;

  static constexpr int32_t DEFAULT_TIME = 300;
  bool operator==(const Input& other) const {
    return dimensions == other.dimensions && turns == other.turns && time == other.time &&
           std::is_permutation(products.begin(), products.end(), other.products.begin()) &&
           std::is_permutation(objects.begin(), objects.end(), other.objects.begin());
  }
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

  Input input;
  input.dimensions = Vec2{json_input["width"], json_input["height"]};
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