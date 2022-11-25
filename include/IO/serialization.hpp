#include <nlohmann/json.hpp>

#include "data_structures/Vec2.hpp"
#include "landscape.hpp"
#include "parsing.hpp"
#include "placeable.hpp"

namespace serialization {

struct Output {
  Vec2 dimensions;
  int32_t turns = 0;
  int32_t time = 0;
  std::vector<Product> products;
  std::vector<LandscapeObject> landscape_objects;
  std::vector<PlaceableObject> placeable_objects;

  bool operator==(const Output& other) const {
    return dimensions == other.dimensions && turns == other.turns && time == other.time &&
           std::is_permutation(products.begin(), products.end(), other.products.begin()) &&
           std::is_permutation(landscape_objects.begin(), landscape_objects.end(),
                               other.landscape_objects.begin()) &&
           std::is_permutation(placeable_objects.begin(), placeable_objects.end(),
                               other.placeable_objects.begin());
  }
};

template <class... Ts>
struct overloaded : Ts... {  // NOLINT(fuchsia-multiple-inheritance)
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

inline nlohmann::json serialize_object(const PlaceableObject& object) {
  return std::visit(overloaded{
                        [&](const Combiner& combiner) -> nlohmann::json {
                          return {{"type", "combiner"},
                                  {"x", combiner.handle.x()},
                                  {"y", combiner.handle.y()},
                                  {"subtype", static_cast<int>(combiner.rotation)}};
                        },
                        [&](const Conveyor3& conveyor3) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", conveyor3.handle.x()},
                                  {"y", conveyor3.handle.y()},
                                  {"subtype", static_cast<int>(conveyor3.rotation)}};
                        },
                        [&](const Conveyor4& conveyor4) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", conveyor4.handle.x()},
                                  {"y", conveyor4.handle.y()},
                                  {"subtype", static_cast<int>(conveyor4.rotation) + 4}};
                        },
                        [&](const Factory& factory) -> nlohmann::json {
                          return {{"type", "factory"},
                                  {"x", factory.handle.x()},
                                  {"y", factory.handle.y()},
                                  {"subtype", static_cast<int>(factory.subtype)}};
                        },
                        [&](const Mine& mine) -> nlohmann::json {
                          return {{"type", "mine"},
                                  {"x", mine.handle.x()},
                                  {"y", mine.handle.y()},
                                  {"subtype", static_cast<int>(mine.rotation)}};
                        },
                    },
                    object);
}

inline nlohmann::json serialize_object(const LandscapeObject& object) {
  return std::visit(overloaded{
                        [&](const Deposit& deposit) -> nlohmann::json {
                          return {{"type", "deposit"},
                                  {"x", deposit.handle.x()},
                                  {"y", deposit.handle.y()},
                                  {"width", deposit.dimensions.x()},
                                  {"height", deposit.dimensions.y()},
                                  {"subtype", static_cast<int>(deposit.subtype)}};
                        },
                        [&](const Obstacle& obstacle) -> nlohmann::json {
                          return {{"type", "obstacle"},
                                  {"x", obstacle.handle.x()},
                                  {"y", obstacle.handle.y()},
                                  {"width", obstacle.dimensions.x()},
                                  {"height", obstacle.dimensions.y()}};
                        },
                    },
                    object);
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
  out["width"] = output.dimensions.x();
  out["height"] = output.dimensions.y();

  nlohmann::json objects = nlohmann::json::array();
  for (const auto& object : output.landscape_objects) {
    objects.emplace_back(serialize_object(object));
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

};  // namespace serialization