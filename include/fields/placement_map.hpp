#pragma once

#include <tuple>

#include "fields/distance_map.hpp"
#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/vec2.hpp"
#include "landscape.hpp"

namespace profit {
enum Placeability : bool {
  INVALID,
  VALID,
};
using PlacementMap = Field<Placeability, INVALID, INVALID>;

using geometry::Coordinate;

/* Assumens handle at top left */
inline std::tuple<PlacementMap, std::vector<Vec2>> placements_for(const Vec2 object_dimensions,
                                                                  const OccupancyMap& occupancy_map,
                                                                  const DistanceMap& distance_map) {
  PlacementMap placements(occupancy_map.dimensions());
  std::vector<Vec2> possible_cells;
  Coordinate last_invalid = -1;

  for (auto y = 0; y < placements.dimensions().y(); ++y) {
    for (Coordinate i = 0 - object_dimensions.x(), j = -1; i < placements.dimensions().x();
         ++i, ++j) {
      if (occupancy_map.at(Vec2{j, y}) != CellOccupancy::EMPTY ||
          distance_map.at(Vec2{j, y}) == NOT_REACHABLE) {
        last_invalid = j;
      }
      if (last_invalid < i) {
        placements.set(Vec2{i, y}, VALID);
      }
    }
  }

  for (auto x = 0; x < placements.dimensions().x(); ++x) {
    for (Coordinate j = -1; j < object_dimensions.y() - 1; ++j) {
      if (placements.at(Vec2{x, j}) == INVALID) {
        last_invalid = j;
      }
    }

    for (Coordinate i = 0, j = object_dimensions.y() - 1; i < placements.dimensions().y();
         ++i, ++j) {
      if (placements.at(Vec2{x, j}) == INVALID) {
        last_invalid = j;
      }
      if (last_invalid < i) {
        possible_cells.emplace_back(x, i);
      } else {
        placements.set(Vec2{x, i}, INVALID);
      }
    }
  }

  return std::make_tuple(placements, possible_cells);
}

}  // namespace profit