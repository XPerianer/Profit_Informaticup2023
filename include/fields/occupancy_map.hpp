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
  for (const auto& deposit : input.deposits) {
    const auto rect = as_rectangle(deposit);
    for (Vec2 coordinate : rect) {
      if (is_on_border(rect, coordinate)) {
        occupancy_map.set(coordinate, CellOccupancy::EGRESS);
      } else {
        occupancy_map.set(coordinate, CellOccupancy::BLOCKED);
      }
    }
  }
  for (const auto& obstacle : input.obstacles) {
    for (Vec2 coordinate : as_rectangle(obstacle)) {
      occupancy_map.set(coordinate, CellOccupancy::BLOCKED);
    }
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

inline void place(const Conveyor3& conveyor, OccupancyMap* occupancy_map) {
  occupancy_map->set(conveyor.ingress(), CellOccupancy::INGRESS);
  occupancy_map->set(conveyor.egress(), CellOccupancy::EGRESS);
  occupancy_map->set(conveyor.handle, CellOccupancy::CONVEYOR_CROSSING);
}

inline void place(const Conveyor4& conveyor, OccupancyMap* occupancy_map) {
  occupancy_map->set(conveyor.ingress(), CellOccupancy::INGRESS);
  occupancy_map->set(conveyor.egress(), CellOccupancy::EGRESS);
  occupancy_map->set(conveyor.handle, CellOccupancy::CONVEYOR_CROSSING);
  if (conveyor.rotation == Rotation::LEFT_TO_RIGHT ||
      conveyor.rotation == Rotation::RIGHT_TO_LEFT) {
    occupancy_map->set(conveyor.handle + Vec2{1, 0}, CellOccupancy::CONVEYOR_CROSSING);
  } else {
    occupancy_map->set(conveyor.handle + Vec2{0, 1}, CellOccupancy::CONVEYOR_CROSSING);
  }
}

inline void place(const Factory factory, OccupancyMap* occupancy_map) {
  const Rectangle shape = as_rectangle(factory);
  for (auto coordinate : shape) {
    if (geometry::is_on_border(shape, coordinate)) {
      occupancy_map->set(coordinate, CellOccupancy::INGRESS);
    } else {
      occupancy_map->set(coordinate, CellOccupancy::BLOCKED);
    }
  }
}

}  // namespace profit
