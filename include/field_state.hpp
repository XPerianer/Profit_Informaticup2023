#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "fields/placement_map.hpp"
#include "geometry/coordinate.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "product.hpp"

namespace profit {
using geometry::Coordinate;

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
                                              const std::vector<DistanceMap>& distance_maps,
                                              FieldState& state) {
  auto [possible_placements, possible_cells] =
      placements_for(Factory::DIMENSIONS, state.occupancy_map, distance_maps[0]);

  if (possible_cells.empty()) {
    return std::nullopt;
  }

  auto min_distance_to_reach_factory_at = [&](const Vec2 handle, const DistanceMap& distance_map) {
    DistanceT min = distance_map.at(handle);
    for (Coordinate offset = 0; offset < Factory::DIMENSIONS.width(); ++offset) {
      min = std::min(min, distance_map.at(handle + Vec2{offset, 0}));
      min = std::min(min, distance_map.at(handle + Vec2{offset, Factory::DIMENSIONS.height() - 1}));
    }
    for (Coordinate offset = 1; offset < Factory::DIMENSIONS.height() - 1; ++offset) {
      min = std::min(min, distance_map.at(handle + Vec2{0, offset}));
      min = std::min(min, distance_map.at(handle + Vec2{Factory::DIMENSIONS.width() - 1, offset}));
    }
    return min;
  };

  auto min_reachable_by_all = [&](const Vec2 cell) {
    DistanceT min = distance_maps[0].at(cell);
    for (const auto& distance_map : distance_maps) {
      min = std::max(min, min_distance_to_reach_factory_at(cell, distance_map));
    }
    return min;
  };

  Vec2 handle = *std::ranges::min_element(possible_cells, [&](Vec2 cell_a, Vec2 cell_b) {
    return min_reachable_by_all(cell_a) < min_reachable_by_all(cell_b);
  });

  Factory factory = {handle, product};
  place(factory, state.occupancy_map);
  auto placed = static_cast<FactoryId>(state.factories.size());
  state.factories.emplace(placed, factory);
  return placed;
}

}  // namespace profit
