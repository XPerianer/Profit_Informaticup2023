#pragma once

#include <queue>
#include <ranges>
#include <variant>

#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "placeable.hpp"
#include "utils.hpp"

namespace profit {

using DistanceT = int16_t;
constexpr DistanceT NOT_REACHABLE = -1;

using DistanceMap = Field<DistanceT, NOT_REACHABLE, NOT_REACHABLE>;

/**
 *  Sets distance of a cell at ingress if given coordinate is suitable for placing an ingress.
 *  Returns if cell could be set.
 *  Post condition: distances[ingress] = distance.
 */
inline bool set_ingress(DistanceMap& distances, const OccupancyMap& occupancy_map,
                        const Vec2 ingress, DistanceT distance) {
  bool is_occupied = occupancy_map.at(ingress) != CellOccupancy::EMPTY;
  bool was_reached_before = distances.at(ingress) != NOT_REACHABLE;
  bool neighbored_to_egress = any_neighbor_is(occupancy_map, ingress, CellOccupancy::EGRESS);
  if (is_occupied || was_reached_before || neighbored_to_egress) {
    return false;
  }
  distances.set(ingress, distance);
  return true;
}

/**
 * Tries to place object in distances and adds new reacheable cells to reached_ingresses.
 * Post conditions:
 * - adds next ingresses to reached_ingresses
 * - for each viable new ingress, distances[ingress] = distance
 */
template <typename Placeable>
inline void place_object(DistanceMap& distances, const OccupancyMap& occupancy_map,
                         const Placeable object, DistanceT distance,
                         std::queue<Vec2>& reached_ingresses) {
  if (collides(object, occupancy_map)) {
    return;
  }
  for (const Vec2 downstream_ingress_cell : object.downstream_ingress_cells()) {
    if (set_ingress(distances, occupancy_map, downstream_ingress_cell, distance)) {
      reached_ingresses.emplace(downstream_ingress_cell);
    }
  }
}

/**
 * See place_object
 */
inline void place_mine(DistanceMap& distances, const OccupancyMap& occupancy_map, const Mine mine,
                       DistanceT distance, std::queue<Vec2>& reached_ingresses) {
  place_object<Mine>(distances, occupancy_map, mine, distance, reached_ingresses);
}

/**
 * See place_object
 */
inline void place_conveyor_3(DistanceMap& distances, const OccupancyMap& occupancy_map,
                             const Conveyor3 conveyor, DistanceT distance,
                             std::queue<Vec2>& reached_ingresses) {
  place_object<Conveyor3>(distances, occupancy_map, conveyor, distance, reached_ingresses);
}

/**
 * See place_object
 */
inline void place_conveyor_4(DistanceMap& distances, const OccupancyMap& occupancy_map,
                             const Conveyor4 conveyor, DistanceT distance,
                             std::queue<Vec2>& reached_ingresses) {
  place_object<Conveyor4>(distances, occupancy_map, conveyor, distance, reached_ingresses);
}

/**
 * See place_object
 */
inline void place_combiner(DistanceMap& distances, const OccupancyMap& occupancy_map,
                           const Combiner combiner, DistanceT distance,
                           std::queue<Vec2>& reached_ingresses) {
  bool ingresses_connect_to_egress =
      std::ranges::any_of(combiner.ingresses(), [&](const Vec2 ingress) {
        return any_neighbor_is(occupancy_map, ingress, CellOccupancy::EGRESS);
      });
  if (!ingresses_connect_to_egress) {
    place_object<Combiner>(distances, occupancy_map, combiner, distance, reached_ingresses);
  }
}

/**
 * For a given mined ingress position, add all possible movements (by conveyor or combiner in any
 * direction) to distances. Post conditions:
 * - adds next ingresses to reached_ingresses
 * - for each viables new ingress, distances[ingress] = distance
 */
inline void move_by_one(DistanceMap& distances, const OccupancyMap& occupancy_map,
                        Vec2 reached_ingress, std::queue<Vec2>& reached_ingresses) {
  auto next_distance = static_cast<DistanceT>(distances.at(reached_ingress) + 1);

  for (auto rotation : ROTATIONS) {
    place_conveyor_3(distances, occupancy_map, Conveyor3::with_ingress(reached_ingress, rotation),
                     next_distance, reached_ingresses);
    place_conveyor_4(distances, occupancy_map, Conveyor4::with_ingress(reached_ingress, rotation),
                     next_distance, reached_ingresses);
    place_combiner(distances, occupancy_map, Combiner::with_left_ingress(reached_ingress, rotation),
                   next_distance, reached_ingresses);
    place_combiner(distances, occupancy_map,
                   Combiner::with_right_ingress(reached_ingress, rotation), next_distance,
                   reached_ingresses);
  }
}

/* Returns an approximation */
inline DistanceMap distances_from(const Deposit& deposit, const OccupancyMap& occupancy_map) {
  DistanceMap distances(occupancy_map.dimensions());
  // Invariant: For each cell in the queue: We've reached this cell in (distance) steps.
  // We can place objects with an ingress there.
  std::queue<Vec2> reached_ingresses;

  for (Vec2 possible_ingress_location : outer_connected_border_cells(as_rectangle(deposit))) {
    for (auto rotation : ROTATIONS) {
      place_mine(distances, occupancy_map, Mine::with_ingress(possible_ingress_location, rotation),
                 1, reached_ingresses);
    }
  }

  while (!reached_ingresses.empty()) {
    Vec2 reached_ingress = reached_ingresses.front();
    reached_ingresses.pop();
    move_by_one(distances, occupancy_map, reached_ingress, reached_ingresses);
  }

  return distances;
}

}  // namespace profit
