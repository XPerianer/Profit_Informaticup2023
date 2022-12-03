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

inline OccupancyMap create_from(const parsing::Input& input) {
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
                                   const auto rect = as_rectangle(obstacle);
                                   for (Vec2 coordinate : rect) {
                                     occupancy_map.set(coordinate, CellOccupancy::BLOCKED);
                                   }
                                 }},
               object);
  }
  return occupancy_map;
}

template <typename Collidable>
inline bool collides(const Collidable& collidable, const OccupancyMap& occupancy_map,
                     bool allow_conveyor_crossings = false) {
  return std::ranges::any_of(collidable.occupied_cells(), [&](const Vec2 cell) {
    auto occupancy = occupancy_map.at(cell);
    return (occupancy != CellOccupancy::EMPTY && occupancy != CellOccupancy::CONVEYOR_CROSSING) ||
           (occupancy != CellOccupancy::EMPTY && !allow_conveyor_crossings);
  });
}

}  // namespace profit