#include <bits/ranges_algo.h>

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
  // naiv: finde die location mit der niedrigsten distanz, schaue ob es da hin passt, eventuell mit
  // ein bisschen rückeln wenn rütteln nicht klappt oder zu sehr entfernt, gehe weiter
  std::vector<Vec2> suitable_cells;
  DistanceMap merged_distances = merge(distance_maps);
  auto [possible_placements, possible_cells] =
      placements_for(Factory::DIMENSIONS, state.occupancy_map, merged_distances);

  if (possible_cells.empty()) {
    return std::nullopt;
  }

  // besser wäre, sich die ränder einer potenziellen fabrik anzusehen und da, wo summe aller
  // ränder am geringsten, ist der optimale platz, da so diese hot spots mit enttarnt werden ->
  // neeee, siehe task 4
  Vec2 handle = *std::min_element(
      possible_cells.begin(), possible_cells.end(), [&](Vec2 cell_a, Vec2 cell_b) {
        return merged_distances.at(cell_a) < merged_distances.at(cell_b);
      });

  // todo: auslagern
  Factory factory = {handle, product};
  for (auto y = 0; y < Factory::DIMENSIONS.y(); ++y) {
    for (auto x = 0; x < Factory::DIMENSIONS.x(); ++x) {
      if (y == 0 || y == Factory::DIMENSIONS.y() - 1 || x == 0 ||
          x == Factory::DIMENSIONS.x() - 1) {
        state.occupancy_map.set(Vec2{x, y}, CellOccupancy::INGRESS);
      } else {
        state.occupancy_map.set(Vec2{x, y}, CellOccupancy::BLOCKED);
      }
    }
  }

  auto placed = static_cast<FactoryId>(state.factories.size());
  state.factories.emplace(placed, factory);
  return placed;
}

}  // namespace profit
