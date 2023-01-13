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
#include "landscape.hpp"
#include "placeable.hpp"
#include "product.hpp"
#include "resource_type.hpp"
#include "subtype.hpp"

namespace profit {

// playing field is at max 100*100 big, so 16 bytes are enough to address every field with a single
// index
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

inline std::optional<std::vector<profit::PlaceableObject>> backtrack_parts(
    geometry::Vec2 ending_egress, PredecessorMap& predecessors, PredecessorMap& object_connections,
    OccupancyMap* occupancy_map) {
  std::vector<PlaceableObject> parts;
  auto width = static_cast<geometry::Coordinate::UnderlyingT>(predecessors.dimensions().width());

  auto object_egress = ending_egress;
  auto object_ingress = Vec2::from_scalar_index(object_connections.at(object_egress), width);

  auto finished = false;

  while (true) {
    // We now need to find out which placable we need to take to connect stuff
    auto rotation = get_rotation_between(object_ingress, object_egress);
    // First check if this is the last connection, then we need a mine
    if (predecessors.at(object_ingress) == INVALID_FIELD) {
      // Put a mine
      parts.emplace_back(Mine::with_ingress(object_ingress, rotation));
      finished = true;
    } else {
      // Otherwise we can infer from the distance
      auto distance = geometry::manhattan_distance(object_egress, object_ingress);

      if (distance == 2) {
        parts.emplace_back(Conveyor3::with_ingress(object_ingress, rotation));
      } else if (distance == 3) {
        parts.emplace_back(profit::Conveyor4::with_ingress(object_ingress, rotation));
      } else {
        FAIL("Unexpected distance between two ingresses, cannot reconstruct placable object.");
      }
    }

    // Check for collisions
    if (collides(parts.back(), *occupancy_map)) {
      parts.pop_back();
      for (auto& part : parts) {
        remove(part, occupancy_map);
      }
      return std::nullopt;
    }

    if (finished) {
      return parts;
    }
    object_egress = Vec2::from_scalar_index(predecessors.at(object_ingress), width);
    object_ingress = Vec2::from_scalar_index(object_connections.at(object_egress), width);

    place(parts.back(), occupancy_map);
  }
}

// TODO: deduplicate with distance_map.hpp see #28
inline std::optional<PipelineId> connect(const Deposit deposit, const FactoryId factory_id,
                                         FieldState* state) {
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
    if (id == factory_id) {
      for (const auto& placeable : pipeline.parts) {
        std::visit(
            utils::Overloaded{[&](const Mine& mine) {
                                for (auto egress : mine.upstream_egress_cells()) {
                                  target_egress_fields.safe_set(egress, TARGET);
                                }
                              },
                              [](const Combiner&) { /* TODO */ },
                              [](const Factory&) { FAIL("Pipeline should not contain a factory"); },
                              [&](const Conveyor3& conveyor) {
                                for (auto egress : conveyor.upstream_egress_cells()) {
                                  target_egress_fields.safe_set(egress, TARGET);
                                }
                              },
                              [&](const Conveyor4& conveyor) {
                                for (auto egress : conveyor.upstream_egress_cells()) {
                                  target_egress_fields.safe_set(egress, TARGET);
                                }
                              }},
            placeable);
      };
    }
  }

  auto connected_egress = calculate_path(deposit, target_egress_fields, &predecessors,
                                         &object_connections, state->occupancy_map);
  if (!connected_egress) {
    return std::nullopt;
  }
  auto parts =
      backtrack_parts(*connected_egress, predecessors, object_connections, &state->occupancy_map);
  // Potential problem: Dijkstra could self-intersect, check here again and if there was a
  // self-intersection, we say that we can not build that
  // TODO: We could recover more gracefully, for example by placing everything that can be placed
  // and then searching for non intersecting connections see #27
  if (!parts) {
    return std::nullopt;
  }

  Pipeline pipeline;
  pipeline.factory_id = factory_id;
  pipeline.parts = *parts;

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
    std::cerr << "final set object.egress(), ingress_index: " << object.egress() << ingress
              << std::endl;
    std::cerr << "object.ingress" << object.ingress() << std::endl;
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
    std::cerr << "Adding object from" << object.ingress() << " to " << object.egress() << std::endl;
    reached_ingresses->emplace(downstream_ingress_cell);
    std::cerr << "added: " << downstream_ingress_cell << std::endl;
  }
  return std::nullopt;
}

inline std::optional<Vec2> calculate_path(Deposit deposit, TargetMap& target_egress_fields,
                                          PredecessorMap* predecessors,
                                          PredecessorMap* object_connections,
                                          OccupancyMap& occupancy_map) {
  std::queue<Vec2> reached_ingresses;
  // Step 1: Go through all mines and add possible ingresses in queue
  for (Vec2 possible_ingress_location :
       geometry::outer_connected_border_cells(as_rectangle(deposit))) {
    for (auto rotation : ROTATIONS) {
      if (auto connected_egress = visit_location_if_placable(
              possible_ingress_location, target_egress_fields, predecessors, object_connections,
              occupancy_map, Mine::with_ingress(possible_ingress_location, rotation),
              &reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
    }
  }

  // Step 2: Loop over all
  // ingresses in queue and add possible ingresses based on the objects we can potentially
  // place, terminating when we have reached desired factory field
  while (!reached_ingresses.empty()) {
    Vec2 ingress = reached_ingresses.front();
    reached_ingresses.pop();

    for (auto rotation : ROTATIONS) {
      if (auto connected_egress = visit_location_if_placable(
              ingress, target_egress_fields, predecessors, object_connections, occupancy_map,
              Conveyor3::with_ingress(ingress, rotation), &reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
      if (auto connected_egress = visit_location_if_placable(
              ingress, target_egress_fields, predecessors, object_connections, occupancy_map,
              Conveyor4::with_ingress(ingress, rotation), &reached_ingresses);
          connected_egress) {
        return connected_egress;
      }
    }
  }
  return std::nullopt;
}
}  // namespace profit
