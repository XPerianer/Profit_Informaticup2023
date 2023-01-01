#include <nlohmann/json.hpp>
#include <ranges>
#include <vector>

#include "io/parsing.hpp"
#include "landscape.hpp"
#include "placeable.hpp"
#include "utils.hpp"

namespace profit::serialization {

struct Output {
  Vec2 dimensions;
  uint64_t turns;
  uint64_t time;
  std::vector<Product> products;
  std::vector<Deposit> deposits;
  std::vector<Obstacle> obstacles;
  std::vector<PlaceableObject> placeable_objects;

  bool operator==(const Output& other) const {
    return dimensions == other.dimensions && turns == other.turns && time == other.time &&
           std::ranges::is_permutation(products, other.products) &&
           std::ranges::is_permutation(deposits, other.deposits) &&
           std::ranges::is_permutation(obstacles, other.obstacles) &&
           std::ranges::is_permutation(placeable_objects, other.placeable_objects);
  }
};

inline nlohmann::json serialize_object(const PlaceableObject& object) {
  return std::visit(utils::Overloaded{
                        [&](const Combiner& combiner) -> nlohmann::json {
                          return {{"type", "combiner"},
                                  {"x", static_cast<int>(combiner.handle.x())},
                                  {"y", static_cast<int>(combiner.handle.y())},
                                  {"subtype", static_cast<int>(combiner.rotation)}};
                        },
                        [&](const Conveyor3& conveyor3) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", static_cast<int>(conveyor3.handle.x())},
                                  {"y", static_cast<int>(conveyor3.handle.y())},
                                  {"subtype", static_cast<int>(conveyor3.rotation)}};
                        },
                        [&](const Conveyor4& conveyor4) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", static_cast<int>(conveyor4.handle.x())},
                                  {"y", static_cast<int>(conveyor4.handle.y())},
                                  {"subtype", static_cast<int>(conveyor4.rotation) + 4}};
                        },
                        [&](const Factory& factory) -> nlohmann::json {
                          return {{"type", "factory"},
                                  {"x", static_cast<int>(factory.handle.x())},
                                  {"y", static_cast<int>(factory.handle.y())},
                                  {"subtype", static_cast<int>(factory.type)}};
                        },
                        [&](const Mine& mine) -> nlohmann::json {
                          return {{"type", "mine"},
                                  {"x", static_cast<int>(mine.handle.x())},
                                  {"y", static_cast<int>(mine.handle.y())},
                                  {"subtype", static_cast<int>(mine.rotation)}};
                        },
                    },
                    object);
}

inline nlohmann::json serialize_object(const Deposit& deposit) {
  return {{"type", "deposit"},
          {"x", static_cast<int>(deposit.handle.x())},
          {"y", static_cast<int>(deposit.handle.y())},
          {"width", static_cast<int>(deposit.dimensions.width())},
          {"height", static_cast<int>(deposit.dimensions.height())},
          {"subtype", static_cast<int>(deposit.type)}};
};

inline nlohmann::json serialize_object(const Obstacle& obstacle) {
  return {{"type", "obstacle"},
          {"x", static_cast<int>(obstacle.handle.x())},
          {"y", static_cast<int>(obstacle.handle.y())},
          {"width", static_cast<int>(obstacle.dimensions.x())},
          {"height", static_cast<int>(obstacle.dimensions.y())}};
}

inline nlohmann::json serialize_product(const Product& product) {
  return {{"type", "product"},
          {"subtype", static_cast<int>(product.type)},
          {"resources", product.requirements.as_array()},
          {"points", product.points}};
}

inline nlohmann::json serialize(const std::vector<PlaceableObject>& objects) {
  nlohmann::json out = nlohmann::json::array();
  for (const auto& object : objects) {
    out.emplace_back(serialize_object(object));
  }
  return out;
}

inline nlohmann::json serialize_detailed(const Output& output) {
  nlohmann::json out;
  out["width"] = static_cast<int>(output.dimensions.x());
  out["height"] = static_cast<int>(output.dimensions.y());

  nlohmann::json objects = nlohmann::json::array();
  for (const auto& deposit : output.deposits) {
    objects.emplace_back(serialize_object(deposit));
  }
  for (const auto& obstacle : output.obstacles) {
    objects.emplace_back(serialize_object(obstacle));
  }
  for (const auto& object : output.placeable_objects) {
    objects.emplace_back(serialize_object(object));
  }
  out["objects"] = objects;

  nlohmann::json products = nlohmann::json::array();
  for (const auto& product : output.products) {
    products.emplace_back(serialize_product(product));
  }
  out["products"] = products;

  out["turns"] = output.turns;
  out["time"] = output.time;
  return out;
}

}  // namespace profit::serialization
