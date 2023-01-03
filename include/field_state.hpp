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

const auto INVALID_PIPELINE_ID = std::numeric_limits<PipelineId>::max();

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

static FieldState from_input(const parsing::Input &input) {
  FieldState state {
    .occupancy_map = occupancies_from(input),
    .pipelines = {},
    .factories = {},
  };
  return state;
}


}  // namespace profit
