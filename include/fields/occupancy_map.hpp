#pragma once

#include <variant>

#include "fields/field.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "objects/landscape.hpp"

namespace profit {

enum class CellOccupancy {
  EMPTY,
  BLOCKED,
  CONVEYOR_CROSSING,
  DOUBLE_CONVEYOR_CROSSING,
  INGRESS,
  EGRESS,
};

using OccupancyMap = Field<CellOccupancy, CellOccupancy::BLOCKED, CellOccupancy::EMPTY>;

[[nodiscard]] inline OccupancyMap occupancies_from(const parsing::Input& input) {
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
[[nodiscard]] inline bool collides(const Placeable& object, const OccupancyMap& occupancy_map) {
  return std::ranges::any_of(object.occupied_cells(),
                             [&](const Vec2 cell) {
                               auto occupancy = occupancy_map.at(cell);
                               if constexpr (std::is_same_v<Placeable, Conveyor4> ||
                                             std::is_same_v<Placeable, Conveyor3>) {
                                 return occupancy != CellOccupancy::EMPTY &&
                                        (occupancy != CellOccupancy::CONVEYOR_CROSSING ||
                                         !object.can_overlap_at(cell));
                               } else {
                                 return occupancy != CellOccupancy::EMPTY;
                               }
                             }) ||
         any_neighbor_count(occupancy_map, object.egress(), CellOccupancy::INGRESS) > 1;
}

template <>
[[nodiscard]] inline bool collides(const Factory& object, const OccupancyMap& occupancy_map) {
  return std::ranges::any_of(object.occupied_cells(), [&](const Vec2 cell) {
    auto occupancy = occupancy_map.at(cell);
    return occupancy != CellOccupancy::EMPTY;
  });
}

template <>
[[nodiscard]] inline bool collides(const PlaceableObject& object,
                                   const OccupancyMap& occupancy_map) {
  return std::visit([&](const auto& placable) { return collides(placable, occupancy_map); },
                    object);
}

// NOLINTNEXTLINE(misc-no-recursion)
inline void place(const PlaceableObject& object, OccupancyMap* occupancy_map) {
  // NOLINTNEXTLINE(misc-no-recursion)
  std::visit([&](const auto& placable) { place(placable, occupancy_map); }, object);
}

inline void place(const Mine& mine, OccupancyMap* occupancy_map) {
  for (auto occupied : mine.occupied_cells()) {
    occupancy_map->set(occupied, CellOccupancy::BLOCKED);
  }
  occupancy_map->set(mine.ingress(), CellOccupancy::INGRESS);
  occupancy_map->set(mine.egress(), CellOccupancy::EGRESS);
}

inline void place_on_conveyor_crossing(const Vec2& crossing, OccupancyMap* occupancy_map) {
  if (occupancy_map->at(crossing) == CellOccupancy::CONVEYOR_CROSSING) {
    occupancy_map->set(crossing, CellOccupancy::DOUBLE_CONVEYOR_CROSSING);
  } else {
    occupancy_map->set(crossing, CellOccupancy::CONVEYOR_CROSSING);
  }
}

inline void place(const Conveyor3& conveyor, OccupancyMap* occupancy_map) {
  occupancy_map->set(conveyor.ingress(), CellOccupancy::INGRESS);
  occupancy_map->set(conveyor.egress(), CellOccupancy::EGRESS);
  place_on_conveyor_crossing(conveyor.handle, occupancy_map);
}

inline void place(const Conveyor4& conveyor, OccupancyMap* occupancy_map) {
  occupancy_map->set(conveyor.ingress(), CellOccupancy::INGRESS);
  occupancy_map->set(conveyor.egress(), CellOccupancy::EGRESS);
  place_on_conveyor_crossing(conveyor.handle, occupancy_map);
  if (conveyor.rotation == Rotation::LEFT_TO_RIGHT ||
      conveyor.rotation == Rotation::RIGHT_TO_LEFT) {
    place_on_conveyor_crossing(conveyor.handle + Vec2{1, 0}, occupancy_map);
  } else {
    place_on_conveyor_crossing(conveyor.handle + Vec2{0, 1}, occupancy_map);
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

template <typename Placable>
inline void remove(const Placable& object, OccupancyMap* occupancy_map) {
  for (auto cell : object.occupied_cells()) {
    occupancy_map->set(cell, CellOccupancy::EMPTY);
  }
}

inline void remove_on_conveyor_crossing(const Vec2& crossing, OccupancyMap* occupancy_map) {
  if (occupancy_map->at(crossing) == CellOccupancy::DOUBLE_CONVEYOR_CROSSING) {
    occupancy_map->set(crossing, CellOccupancy::CONVEYOR_CROSSING);
  } else {
    occupancy_map->set(crossing, CellOccupancy::EMPTY);
  }
}

template <>
inline void remove(const Conveyor3& object, OccupancyMap* occupancy_map) {
  occupancy_map->set(object.ingress(), CellOccupancy::EMPTY);
  occupancy_map->set(object.egress(), CellOccupancy::EMPTY);
  remove_on_conveyor_crossing(object.handle, occupancy_map);
}

template <>
inline void remove(const Conveyor4& object, OccupancyMap* occupancy_map) {
  occupancy_map->set(object.ingress(), CellOccupancy::EMPTY);
  occupancy_map->set(object.egress(), CellOccupancy::EMPTY);
  remove_on_conveyor_crossing(object.handle, occupancy_map);
  if (object.rotation == Rotation::LEFT_TO_RIGHT || object.rotation == Rotation::RIGHT_TO_LEFT) {
    remove_on_conveyor_crossing(object.handle + Vec2{1, 0}, occupancy_map);
  } else {
    remove_on_conveyor_crossing(object.handle + Vec2{0, 1}, occupancy_map);
  }
}

template <>
inline void remove(const PlaceableObject& object, OccupancyMap* occupancy_map) {
  // NOLINTNEXTLINE(misc-no-recursion)
  std::visit([&](const auto& placable) { remove(placable, occupancy_map); }, object);
}

}  // namespace profit
