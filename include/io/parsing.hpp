#pragma once
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "miscellany/assert.hpp"
#include "objects/landscape.hpp"
#include "objects/placeable.hpp"
#include "objects/product.hpp"

namespace profit::parsing {

struct Input {
  Vec2 dimensions;
  uint64_t turns = 0;
  uint64_t time = 0;
  std::vector<Product> products;
  std::vector<Deposit> deposits;
  std::vector<Obstacle> obstacles;

  static constexpr uint64_t DEFAULT_TIME = 300;
  bool operator==(const Input& other) const {
    return dimensions == other.dimensions && turns == other.turns && time == other.time &&
           std::ranges::is_permutation(products, other.products) &&
           std::ranges::is_permutation(deposits, other.deposits) &&
           std::ranges::is_permutation(obstacles, other.obstacles);
  }
};

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
    Product product{type, requirements, product_json["points"]};
    if (product.points != 0) {
      input.products.push_back(product);
    }
  }

  for (const auto& object_json : json_input["objects"]) {
    std::visit(utils::Overloaded{[&](Deposit deposit) { input.deposits.push_back(deposit); },
                                 [&](Obstacle obstacle) { input.obstacles.push_back(obstacle); }},
               parse_object(object_json));
  }

  input.time = json_input.value("time", Input::DEFAULT_TIME);

  return input;
}
}  // namespace profit::parsing
