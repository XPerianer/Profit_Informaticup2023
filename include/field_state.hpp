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
  PlacementMap handle_placements =
      placements_for<Factory::DIMENSIONS>(state->occupancy_map, cc_merged_distances);

  PlacementMap ingress_placements(handle_placements.dimensions());
  for (auto cell : handle_placements) {
    if (handle_placements.at(cell) == INVALID) {
      continue;
    }

    for (Coordinate offset = 0; offset < Factory::DIMENSIONS.width(); ++offset) {
      ingress_placements.set(cell + Vec2{offset, 0}, VALID);
      ingress_placements.set(cell + Vec2{offset, Factory::DIMENSIONS.height() - 1}, VALID);
    }
    for (Coordinate offset = 1; offset < Factory::DIMENSIONS.height() - 1; ++offset) {
      ingress_placements.set(cell + Vec2{0, offset}, VALID);
      ingress_placements.set(cell + Vec2{Factory::DIMENSIONS.width() - 1, offset}, VALID);
    }
  }

  std::vector<std::pair<Vec2, DistanceT>> reachability;

  for (auto cell : cc_merged_distances) {
    if (ingress_placements.at(cell) == INVALID) {
      continue;
    }
    reachability.emplace_back(cell, cc_merged_distances.at(cell));
  }

  std::ranges::sort(reachability,
                    [&](auto pair_a, auto pair_b) { return pair_a.second < pair_b.second; });

  for (auto pair : reachability) {
    if (// can be placed here //ingress -> handle mapping missing ) {
      Factory factory = {pair.first, product};
      place(factory, &state->occupancy_map);
      auto placed = static_cast<FactoryId>(state->factories.size());
      state->factories.emplace(placed, factory);
      return placed;
  }
}

return std::nullopt;
}

}  // namespace profit
