#include <algorithm>
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
using FactoryId = int16_t;
using PipelineId = int16_t;

struct Pipeline {
  FactoryId factory;
  std::vector<PlaceableObject> parts;
};

struct FieldState {
  OccupancyMap occupancy_map;
  // TODO: zelle zu pipeline / factoryindex, damit man nachgucken kann, was einen gerade blockiert
  std::unordered_map<PipelineId, Pipeline> pipelines;
  std::unordered_map<FactoryId, Factory> factories;
};

inline std::optional<FactoryId> place_factory(ProductType product,
                                              const DistanceMap& cc_merged_distances,
                                              FieldState* state) {
  PlacementMap placements =
      placements_for<Factory::DIMENSIONS>(state->occupancy_map, cc_merged_distances);

  Vec2 candidate = Vec2{-1, -1};
  Rectangle latest_placement_shape = Rectangle::from_top_left_and_dimensions(candidate, Vec2{1, 1});

  for (auto cell : cc_merged_distances) {
    if (placements.at(cell) == VALID) {
      latest_placement_shape = Rectangle::from_top_left_and_dimensions(cell, Factory::DIMENSIONS);
    }
    if (!geometry::is_on_border(latest_placement_shape, cell)) {
      continue;
    }
    if (cc_merged_distances.at(cell) < cc_merged_distances.at(candidate)) {
      candidate = latest_placement_shape.top_left();
    }
  }

  if (candidate.x() < 0) {
    return std::nullopt;
  }

  Factory factory = {candidate, product};
  place(factory, &state->occupancy_map);
  auto placed = static_cast<FactoryId>(state->factories.size());
  state->factories.emplace(placed, factory);
  return placed;
}

}  // namespace profit
