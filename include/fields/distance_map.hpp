#pragma once

#include <queue>
#include <ranges>
#include <set>
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

template <typename PlaceableT>
inline void update_reachability_if_placeable(DistanceMap& distances,
                                             const OccupancyMap& occupancy_map,
                                             const PlaceableT object, DistanceT distance,
                                             std::queue<Vec2>& reached_ingresses,
                                             std::set<Vec2>& reached_egresses) {
  if (collides(object, occupancy_map)) {
    return;
  }
  for (const Vec2 downstream_ingress_cell : object.downstream_ingress_cells()) {
    bool is_occupied = occupancy_map.at(downstream_ingress_cell) != CellOccupancy::EMPTY;
    bool is_egress = occupancy_map.at(downstream_ingress_cell) == CellOccupancy::EGRESS;
    bool was_reached_before = distances.at(downstream_ingress_cell) != NOT_REACHABLE;
    bool neighbored_to_egress =
        any_neighbor_is(occupancy_map, downstream_ingress_cell, CellOccupancy::EGRESS);
    if (is_egress) {
      reached_egresses.emplace(downstream_ingress_cell);
    }
    if (is_occupied || was_reached_before || neighbored_to_egress) {
      continue;
    }
    distances.set(downstream_ingress_cell, distance);
    reached_ingresses.emplace(downstream_ingress_cell);
  }
}

/* Returns an approximation */
inline DistanceMap distances_from(const Deposit& deposit, const OccupancyMap& occupancy_map,
                                  std::set<Vec2>& reached_egresses) {
  DistanceMap distances(occupancy_map.dimensions());
  // Invariant: For each cell in the queue: We've reached this cell in (distance) steps.
  // We can place objects with an ingress there.
  std::queue<Vec2> reached_ingresses;

  for (Vec2 possible_ingress_location : outer_connected_border_cells(as_rectangle(deposit))) {
    for (auto rotation : ROTATIONS) {
      update_reachability_if_placeable(distances, occupancy_map,
                                       Mine::with_ingress(possible_ingress_location, rotation), 1,
                                       reached_ingresses, reached_egresses);
    }
  }

  while (!reached_ingresses.empty()) {
    Vec2 reached_ingress = reached_ingresses.front();
    reached_ingresses.pop();
    auto next_distance = static_cast<DistanceT>(distances.at(reached_ingress) + 1);

    for (auto rotation : ROTATIONS) {
      update_reachability_if_placeable(distances, occupancy_map,
                                       Conveyor3::with_ingress(reached_ingress, rotation),
                                       next_distance, reached_ingresses, reached_egresses);
      update_reachability_if_placeable(distances, occupancy_map,
                                       Conveyor4::with_ingress(reached_ingress, rotation),
                                       next_distance, reached_ingresses, reached_egresses);
      for (auto combiner : {Combiner::with_left_ingress(reached_ingress, rotation),
                            Combiner::with_right_ingress(reached_ingress, rotation)}) {
        bool ingresses_connect_to_egress =
            std::ranges::any_of(combiner.ingresses(), [&](const Vec2 ingress) {
              return any_neighbor_is(occupancy_map, ingress, CellOccupancy::EGRESS);
            });
        if (!ingresses_connect_to_egress) {
          update_reachability_if_placeable(distances, occupancy_map, combiner, next_distance,
                                           reached_ingresses, reached_egresses);
        }
      }
    }
  }

  return distances;
}

}  // namespace profit
