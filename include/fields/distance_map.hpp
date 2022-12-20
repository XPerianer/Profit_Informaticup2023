#pragma once

#include <map>
#include <queue>
#include <ranges>
#include <variant>

#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "placeable.hpp"
#include "utils.hpp"

namespace profit {

using DistanceT = int16_t;
constexpr DistanceT NOT_REACHABLE = -1;

using DistanceMap = Field<DistanceT, NOT_REACHABLE, NOT_REACHABLE>;

using geometry::Coordinate;
using geometry::Rectangle;

using DepositId = int16_t;
using ConnectedComponent = std::vector<DepositId>;

template <typename PlaceableT>
inline void update_reachability_if_placeable(DistanceMap& distances,
                                             const OccupancyMap& occupancy_map,
                                             const PlaceableT object, DistanceT distance,
                                             std::queue<Vec2>& reached_ingresses) {
  if (collides(object, occupancy_map)) {
    return;
  }
  for (const Vec2 downstream_ingress_cell : object.downstream_ingress_cells()) {
    bool is_occupied = occupancy_map.at(downstream_ingress_cell) != CellOccupancy::EMPTY;
    bool was_reached_before = distances.at(downstream_ingress_cell) != NOT_REACHABLE;
    bool neighbored_to_egress =
        any_neighbor_is(occupancy_map, downstream_ingress_cell, CellOccupancy::EGRESS);
    if (is_occupied || was_reached_before || neighbored_to_egress) {
      continue;
    }
    distances.set(downstream_ingress_cell, distance);
    reached_ingresses.emplace(downstream_ingress_cell);
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
      update_reachability_if_placeable(distances, occupancy_map,
                                       Mine::with_ingress(possible_ingress_location, rotation), 1,
                                       reached_ingresses);
    }
  }

  while (!reached_ingresses.empty()) {
    Vec2 reached_ingress = reached_ingresses.front();
    reached_ingresses.pop();
    auto next_distance = static_cast<DistanceT>(distances.at(reached_ingress) + 1);

    for (auto rotation : ROTATIONS) {
      update_reachability_if_placeable(distances, occupancy_map,
                                       Conveyor3::with_ingress(reached_ingress, rotation),
                                       next_distance, reached_ingresses);
      update_reachability_if_placeable(distances, occupancy_map,
                                       Conveyor4::with_ingress(reached_ingress, rotation),
                                       next_distance, reached_ingresses);
      for (auto combiner : {Combiner::with_left_ingress(reached_ingress, rotation),
                            Combiner::with_right_ingress(reached_ingress, rotation)}) {
        bool ingresses_connect_to_egress =
            std::ranges::any_of(combiner.ingresses(), [&](const Vec2 ingress) {
              return any_neighbor_is(occupancy_map, ingress, CellOccupancy::EGRESS);
            });
        if (!ingresses_connect_to_egress) {
          update_reachability_if_placeable(distances, occupancy_map, combiner, next_distance,
                                           reached_ingresses);
        }
      }
    }
  }

  return distances;
}

inline std::vector<ConnectedComponent> connected_components_from(
    const std::vector<Deposit>& deposits, const std::vector<DistanceMap>& distances) {
  std::vector<ConnectedComponent> connected_components;
  std::map<Vec2, ConnectedComponent> reachable_to_cc;

  const auto find_nearby_reachable = [&](const DistanceMap& map, Deposit deposit) {
    const DistanceT radius = 5;
    const Rectangle search_area = Rectangle::from_top_left_and_dimensions(
        deposit.handle - Vec2{radius, radius}, deposit.dimensions + Vec2{radius * 2, radius * 2});
    const auto reachable =
        std::ranges::find_if(search_area, [&](Vec2 cell) { return map.at(cell) != NOT_REACHABLE; });
    return reachable != geometry::end(search_area) ? *reachable : deposit.handle;
  };

  for (size_t i = 0; i < distances.size(); i++) {
    const auto reacheable_cc = std::ranges::find_if(reachable_to_cc, [&](const auto& entry) {
      return distances[i].at(entry.first) != NOT_REACHABLE;
    });

    if (reacheable_cc != reachable_to_cc.end()) {
      reachable_to_cc[reacheable_cc->first].emplace_back(i);
    } else {
      reachable_to_cc[find_nearby_reachable(distances[i], deposits[i])].emplace_back(i);
    }
  }

  connected_components.reserve(reachable_to_cc.size());
  for (const auto& [key, value] : reachable_to_cc) {
    connected_components.emplace_back(value);
  }

  return connected_components;
}

}  // namespace profit
