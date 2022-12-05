#pragma once
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "assert.hpp"
#include "landscape.hpp"
#include "placeable.hpp"
#include "product.hpp"


namespace profit::parsing {

struct Input {
  Vec2 dimensions;
  int32_t turns = 0;
  int32_t time = 0;
  std::vector<Product> products;
  std::vector<LandscapeObject> objects;

  static constexpr int32_t DEFAULT_TIME = 300;
  bool operator==(const Input& other) const {
    return dimensions == other.dimensions && turns == other.turns && time == other.time &&
           std::ranges::is_permutation(products, other.products) &&
           std::ranges::is_permutation(objects, other.objects);
  }
};

inline std::vector<Deposit> get_deposits(const Input& input) {
  std::vector<Deposit> deposits;
  for (auto const& object : input.objects) {
    std::visit(utils::Overloaded{[&](const Deposit& deposit) { deposits.push_back(deposit); },
                                 [](const Obstacle& /*obstacle*/) {}},
               object);
  }
  return deposits;
}

inline LandscapeObject parse_object(const nlohmann::json& input) {
  std::string type = input["type"];
  Vec2 coordinate = {static_cast<int>(input["x"]), static_cast<int>(input["y"])};
  Vec2 dimensions = {static_cast<int>(input["width"]), static_cast<int>(input["height"])};

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
  input.dimensions =
      Vec2{static_cast<int>(json_input["width"]), static_cast<int>(json_input["height"])};
  input.turns = json_input["turns"];

  for (const auto& product_json : json_input["products"]) {
    auto type = static_cast<ProductType>(static_cast<int>(product_json["subtype"]));
    Requirements requirements(static_cast<std::vector<int>>(product_json["resources"]));
    input.products.push_back({type, requirements, product_json["points"]});
  }

  for (const auto& object_json : json_input["objects"]) {
    input.objects.push_back(parse_object(object_json));
  }

  input.time = json_input.value("time", Input::DEFAULT_TIME);

  return input;
}
}  // namespace profit::parsing