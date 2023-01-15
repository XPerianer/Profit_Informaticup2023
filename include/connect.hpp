#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <ratio>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "field_state.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "io/parsing.hpp"
#include "objects/landscape.hpp"
#include "objects/placeable.hpp"
#include "objects/product.hpp"
#include "objects/resource_type.hpp"
#include "objects/subtype.hpp"

namespace profit {

using FieldIndex = uint16_t;
constexpr auto INVALID_FIELD = std::numeric_limits<FieldIndex>::max();

enum Target : bool {
  TARGET = true,
  NO_TARGET = false,
};
using TargetMap = Field<Target, NO_TARGET, NO_TARGET>;
using PredecessorMap = Field<FieldIndex, INVALID_FIELD, INVALID_FIELD>;

inline std::optional<Vec2> calculate_path(Deposit deposit, TargetMap& target_egress_fields,
                                          PredecessorMap* predecessors,
                                          PredecessorMap* object_connections,
                                          OccupancyMap& occupancy_map);

inline Rotation get_rotation_between(Vec2 start, Vec2 end) {
  auto horizontal_difference = end.x() - start.x();
  auto vertical_difference = end.y() - start.y();
  if (std::abs(static_cast<geometry::Coordinate::UnderlyingT>(horizontal_difference)) >
      std::abs(static_cast<geometry::Coordinate::UnderlyingT>(vertical_difference))) {
    if (horizontal_difference > 0) {
      return Rotation::LEFT_TO_RIGHT;
    }
    return Rotation::RIGHT_TO_LEFT;
  }
  if (vertical_difference > 0) {
    return Rotation::UP_TO_DOWN;
  }
  return Rotation::DOWN_TO_UP;
}

inline std::pair<bool, std::vector<profit::PlaceableObject>> backtrack_parts(
    geometry::Vec2 ending_egress, PredecessorMap& predecessors, PredecessorMap& object_connections,
    OccupancyMap* occupancy_map, bool mine_at_end = true) {
  std::vector<PlaceableObject> parts;
  auto width = static_cast<geometry::Coordinate::UnderlyingT>(predecessors.dimensions().width());

  auto object_egress = ending_egress;
  auto object_ingress = Vec2::from_scalar_index(object_connections.at(object_egress), width);

  auto finished = false;

  while (true) {
    auto rotation = get_rotation_between(object_ingress, object_egress);
    if (predecessors.at(object_ingress) == INVALID_FIELD) {
      finished = true;
    }
    if (finished && mine_at_end) {
      parts.emplace_back(Mine::with_ingress(object_ingress, rotation));
    } else {
      auto distance = geometry::manhattan_distance(object_egress, object_ingress);

      if (distance == 2) {
        parts.emplace_back(Conveyor3::with_ingress(object_ingress, rotation));
      } else if (distance == 3) {
        parts.emplace_back(profit::Conveyor4::with_ingress(object_ingress, rotation));
      } else {
        FAIL("Unexpected distance between two ingresses, cannot reconstruct placable object.");
      }
    }

    if (collides(parts.back(), *occupancy_map)) {
      parts.pop_back();
      return {false, parts};
    }

    if (finished) {
      place(parts.back(), occupancy_map);
      return {true, parts};
    }
    object_egress = Vec2::from_scalar_index(predecessors.at(object_ingress), width);
    object_ingress = Vec2::from_scalar_index(object_connections.at(object_egress), width);

    place(parts.back(), occupancy_map);
  }
}

inline void set_part_as_target(const PlaceableObject& placeable, TargetMap* target_egress_fields) {
  std::visit(
      utils::Overloaded{[&](const Mine& mine) {
                          for (auto egress : mine.upstream_egress_cells()) {
                            target_egress_fields->safe_set(egress, TARGET);
                          }
                        },
                        [](const Combiner&) { /* TODO */ },
                        [](const Factory&) { FAIL("Pipeline should not contain a factory"); },
                        [&](const Conveyor3& conveyor) {
                          for (auto egress : conveyor.upstream_egress_cells()) {
                            target_egress_fields->safe_set(egress, TARGET);
                          }
                        },
                        [&](const Conveyor4& conveyor) {
                          for (auto egress : conveyor.upstream_egress_cells()) {
                            target_egress_fields->safe_set(egress, TARGET);
                          }
                        }},
      placeable);
}

// TODO: deduplicate with distance_map.hpp see #28
inline std::optional<PipelineId> connect(const DepositId& deposit_id, const FactoryId factory_id,
                                         FieldState* state, const parsing::Input& input) {
  Factory factory = state->factories[factory_id];

  // Connection from downstream ingress to egress
  Field<FieldIndex, INVALID_FIELD, INVALID_FIELD> predecessors(state->occupancy_map.dimensions());
  // Connection from object egress to object ingress
  Field<FieldIndex, INVALID_FIELD, INVALID_FIELD> object_connections(
      state->occupancy_map.dimensions());

  // Step 0: check which fields we can reach: this can be either existing pipeline or factory
  // borders
  TargetMap target_egress_fields(predecessors.dimensions());

  for (Vec2 possible_egress_location :
       geometry::outer_connected_border_cells(as_rectangle(factory))) {
    target_egress_fields.safe_set(possible_egress_location, TARGET);
  }

  for (const auto& [id, pipeline] : state->pipelines) {
    if (id != factory_id) {
      continue;
    }
    for (const auto& placeable : pipeline.parts) {
      set_part_as_target(placeable, &target_egress_fields);
    }
  }

  auto deposit = input.deposits[deposit_id];
  auto connected_egress = calculate_path(deposit, target_egress_fields, &predecessors,
                                         &object_connections, state->occupancy_map);
  if (!connected_egress) {
    return std::nullopt;
  }
  auto [finished, parts] =
      backtrack_parts(*connected_egress, predecessors, object_connections, &state->occupancy_map);
  // Potential problem: Dijkstra could self-intersect, check here again and if there was a
  // self-intersection, we say that we can not build that
  // TODO: We could recover more gracefully, for example by placing everything that can be placed
  // and then searching for non intersecting connections see #27
  if (!finished) {
    DEBUG("Self-intersection inside connect\n");

    PredecessorMap predecessors(input.dimensions);
    PredecessorMap object_connections(predecessors.dimensions());
    TargetMap target_to_path(predecessors.dimensions());
    set_part_as_target(parts.back(), &target_to_path);
    // Try to run connect again with all parts without intersections on the field
    auto connected_egress = calculate_path(deposit, target_to_path, &predecessors,
                                           &object_connections, state->occupancy_map);
    if (!connected_egress) {
      for (auto& part : parts) {
        remove(part, &state->occupancy_map);
      }
      return std::nullopt;
    }
    auto [extra_finished, extra_parts] =
        backtrack_parts(*connected_egress, predecessors, object_connections, &state->occupancy_map);
    DEBUG("finished: " << finished << "\n");

    if (!extra_finished) {
      for (auto& part : parts) {
        remove(part, &state->occupancy_map);
      }
      for (auto& part : extra_parts) {
        remove(part, &state->occupancy_map);
      }
      return std::nullopt;
    }
    for (auto part : extra_parts) {
      parts.push_back(part);
    }
  }

  Pipeline pipeline;
  pipeline.deposit_id = deposit_id;
  pipeline.factory_id = factory_id;
  pipeline.parts = parts;

  return state->add_pipeline(pipeline);
}

template <typename PlaceableT>
inline std::optional<Vec2> visit_location_if_placable(
    Vec2 ingress, const TargetMap& target_egress_fields, PredecessorMap* predecessors,
    PredecessorMap* object_connections, const OccupancyMap& occupancy_map, const PlaceableT object,
    std::queue<Vec2>* reached_ingresses) {
  if (collides(object, occupancy_map)) {
    return std::nullopt;
  }
  auto width = occupancy_map.dimensions().width();
  if (target_egress_fields.at(object.egress())) {
    object_connections->set(object.egress(), ingress.to_scalar_index(width));
    return object.egress();
  }
  for (const Vec2 downstream_ingress_cell : object.downstream_ingress_cells()) {
    bool is_occupied = occupancy_map.at(downstream_ingress_cell) != CellOccupancy::EMPTY;
    bool was_reached_before = predecessors->at(downstream_ingress_cell) != INVALID_FIELD;
    bool neighbored_to_egress =
        any_neighbor_is(occupancy_map, downstream_ingress_cell, CellOccupancy::EGRESS);
    if (is_occupied || was_reached_before || neighbored_to_egress) {
      continue;
    }
    predecessors->set(downstream_ingress_cell, object.egress().to_scalar_index(width));
    object_connections->set(object.egress(), ingress.to_scalar_index(width));
    reached_ingresses->emplace(downstream_ingress_cell);
  }
  return std::nullopt;
}

inline std::optional<Vec2> ingresses_from_deposits(std::queue<Vec2>* reached_ingresses,
                                                   Deposit deposit, TargetMap& target_egress_fields,
                                                   PredecessorMap* predecessors,
                                                   PredecessorMap* object_connections,
                                                   OccupancyMap& occupancy_map) {
  for (Vec2 possible_ingress_location :
       geometry::outer_connected_border_cells(as_rectangle(deposit))) {
    for (auto rotation : ROTATIONS) {
      if (auto connected_egress = visit_location_if_placable(
              possible_ingress_location, target_egress_fields, predecessors, object_connections,
              occupancy_map, Mine::with_ingress(possible_ingress_location, rotation),
              reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
    }
  }
  return std::nullopt;
}

inline std::optional<Vec2> connect_ingress_to_target(std::queue<Vec2>* reached_ingresses,
                                                     TargetMap& target_egress_fields,
                                                     PredecessorMap* predecessors,
                                                     PredecessorMap* object_connections,
                                                     OccupancyMap& occupancy_map) {
  while (!reached_ingresses->empty()) {
    Vec2 ingress = reached_ingresses->front();
    reached_ingresses->pop();

    for (auto rotation : ROTATIONS) {
      if (auto connected_egress = visit_location_if_placable(
              ingress, target_egress_fields, predecessors, object_connections, occupancy_map,
              Conveyor3::with_ingress(ingress, rotation), reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
      if (auto connected_egress = visit_location_if_placable(
              ingress, target_egress_fields, predecessors, object_connections, occupancy_map,
              Conveyor4::with_ingress(ingress, rotation), reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
    }
  }
  return std::nullopt;
}

inline std::optional<Vec2> calculate_path(Deposit deposit, TargetMap& target_egress_fields,
                                          PredecessorMap* predecessors,
                                          PredecessorMap* object_connections,
                                          OccupancyMap& occupancy_map) {
  std::queue<Vec2> reached_ingresses;

  auto reached = ingresses_from_deposits(&reached_ingresses, deposit, target_egress_fields,
                                         predecessors, object_connections, occupancy_map);
  if (reached) {
    return reached;
  }

  reached = connect_ingress_to_target(&reached_ingresses, target_egress_fields, predecessors,
                                      object_connections, occupancy_map);
  if (reached) {
    return reached;
  }

  return std::nullopt;
}
}  // namespace profit
