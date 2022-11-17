#include <nlohmann/json.hpp>

#include "placeable.hpp"

namespace serialization {

template <class... Ts>
struct overloaded : Ts... { // NOLINT(fuchsia-multiple-inheritance)
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
                                  {"subtype", combiner.rotation}};
                        },
                        [&](const Conveyor3& conveyor3) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", conveyor3.handle.x()},
                                  {"y", conveyor3.handle.y()},
                                  {"subtype", conveyor3.rotation}};
                        },
                        [&](const Conveyor4& conveyor4) -> nlohmann::json {
                          return {{"type", "conveyor"},
                                  {"x", conveyor4.handle.x()},
                                  {"y", conveyor4.handle.y()},
                                  {"subtype", conveyor4.rotation + 4}};
                        },
                        [&](const Factory& factory) -> nlohmann::json {
                          return {{"type", "factory"},
                                  {"x", factory.handle.x()},
                                  {"y", factory.handle.y()},
                                  {"subtype", factory.subtype}};
                        },
                        [&](const Mine& mine) -> nlohmann::json {
                          return {{"type", "mine"},
                                  {"x", mine.handle.x()},
                                  {"y", mine.handle.y()},
                                  {"subtype", mine.rotation}};
                        },
                    },
                    object);
}

inline nlohmann::json serialize(const std::vector<PlaceableObject>& objects) {
  nlohmann::json out = nlohmann::json::array();
  for (const auto& object : objects) {
    out.emplace_back(serialize_object(object));
  }
  return out;
}
};  // namespace serialization