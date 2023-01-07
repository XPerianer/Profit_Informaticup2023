#pragma once

#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "fields/placement_map.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "product.hpp"

namespace profit {
using geometry::Coordinate;

using FactoryId = int16_t;
using PipelineId = int16_t;

const auto INVALID_PIPELINE_ID = std::numeric_limits<PipelineId>::max();
static PipelineId PIPELINE_INDEX_COUNTER = 0;

struct Pipeline {
  FactoryId factory_id;
  std::vector<PlaceableObject> parts;
};

struct FieldState {
  OccupancyMap occupancy_map;
  // TODO: zelle zu pipeline / factoryindex, damit man nachgucken kann, was einen gerade blockiert
  std::unordered_map<PipelineId, Pipeline> pipelines;
  std::unordered_map<FactoryId, Factory> factories;
  inline PipelineId add_pipeline(Pipeline pipeline);
};

inline PipelineId FieldState::add_pipeline(Pipeline pipeline) {
  auto index = PIPELINE_INDEX_COUNTER++;
  pipelines.insert({index, pipeline});
  for (const PlaceableObject &part : pipeline.parts) {
    std::visit(utils::Overloaded{
                   [&](const Conveyor3 &conveyor) { place(conveyor, &occupancy_map); },
                   [&](const Conveyor4 &conveyor) { place(conveyor, &occupancy_map); },
                   [](const Combiner &) {},
                   [](const Mine &) {},
                   [](const Factory &) {},
               },
               part);
  }
  return index;
}

inline FieldState from_input(const parsing::Input &input) {
  FieldState state{
      occupancies_from(input),
      {},
      {},
  };
  return state;
}

inline std::optional<FactoryId> place_factory(ProductType product,
                                              const DistanceMap &cc_merged_distances,
                                              FieldState *state) {
  PlacementMap handle_placements =
      placements_for<Factory::DIMENSIONS>(state->occupancy_map, cc_merged_distances);

  Vec2 min_distance{-1, -1};
  for (auto cell : handle_placements) {
    if (handle_placements.at(cell) == INVALID) {
      continue;
    }
    if (cc_merged_distances.at(cell) < cc_merged_distances.at(min_distance)) {
      min_distance = cell;
    }
  }

  if (min_distance == Vec2{-1, -1}) {
    return std::nullopt;
  }

  Factory factory = {min_distance, product};
  place(factory, &state->occupancy_map);
  auto placed = static_cast<FactoryId>(state->factories.size());
  state->factories.emplace(placed, factory);

  return placed;
}

}  // namespace profit
