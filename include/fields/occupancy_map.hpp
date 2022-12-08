#pragma once

#include <variant>

#include "fields/field.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "landscape.hpp"

namespace profit {

enum class CellOccupancy {
  EMPTY,
  BLOCKED,
  CONVEYOR_CROSSING,
  INGRESS,
  EGRESS,
};

using OccupancyMap = Field<CellOccupancy, CellOccupancy::BLOCKED, CellOccupancy::EMPTY>;

inline OccupancyMap occupancies_from(const parsing::Input& input) {
  OccupancyMap occupancy_map(input.dimensions);
  for (const auto& object : input.objects) {
    std::visit(utils::Overloaded{[&](const Deposit& deposit) {
                                   const auto rect = as_rectangle(deposit);
                                   for (Vec2 coordinate : rect) {
                                     if (is_on_border(rect, coordinate)) {
                                       occupancy_map.set(coordinate, CellOccupancy::EGRESS);
                                     } else {
                                       occupancy_map.set(coordinate, CellOccupancy::BLOCKED);
                                     }
                                   }
                                 },
                                 [&](const Obstacle& obstacle) {
                                   for (Vec2 coordinate : as_rectangle(obstacle)) {
                                     occupancy_map.set(coordinate, CellOccupancy::BLOCKED);
                                   }
                                 }},
               object);
  }
  return occupancy_map;
}

template <typename Placeable>
inline bool collides(const Placeable& object, const OccupancyMap& occupancy_map) {
  return std::ranges::any_of(object.occupied_cells(), [&](const Vec2 cell) {
    auto occupancy = occupancy_map.at(cell);
    if constexpr (std::is_same_v<Placeable, Conveyor4> || std::is_same_v<Placeable, Conveyor3>) {
      return occupancy != CellOccupancy::EMPTY &&
             (occupancy != CellOccupancy::CONVEYOR_CROSSING || !object.can_overlap_at(cell));
    } else {
      return occupancy != CellOccupancy::EMPTY;
    }
  });
}

}  // namespace profit
