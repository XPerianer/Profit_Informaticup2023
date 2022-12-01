#include <nlohmann/json.hpp>
#include <vector>

#include "landscape.hpp"
#include "parsing.hpp"
#include "placeable.hpp"
#include "utils.hpp"

namespace profit {

namespace serialization {

struct Output {
  Vec2 dimensions;
  int32_t turns;
  int32_t time;
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
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

inline nlohmann::json serialize_object(const PlaceableObject& object) {
  return std::visit(overloaded{
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

inline nlohmann::json serialize_object(const LandscapeObject& object) {
  return std::visit(overloaded{
                        [&](const Deposit& deposit) -> nlohmann::json {
                          return {{"type", "deposit"},
                                  {"x", static_cast<int>(deposit.handle.x())},
                                  {"y", static_cast<int>(deposit.handle.y())},
                                  {"width", static_cast<int>(deposit.dimensions.width())},
                                  {"height", static_cast<int>(deposit.dimensions.height())},
                                  {"subtype", static_cast<int>(deposit.type)}};
                        },
                        [&](const Obstacle& obstacle) -> nlohmann::json {
                          return {{"type", "obstacle"},
                                  {"x", static_cast<int>(obstacle.handle.x())},
                                  {"y", static_cast<int>(obstacle.handle.y())},
                                  {"width", static_cast<int>(obstacle.dimensions.x())},
                                  {"height", static_cast<int>(obstacle.dimensions.y())}};
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
  out["width"] = static_cast<int>(output.dimensions.x());
  out["height"] = static_cast<int>(output.dimensions.y());

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

}  // namespace serialization
}//