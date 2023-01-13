#pragma once

#include "fields/distance_map.hpp"
#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/vec2.hpp"
#include "objects/landscape.hpp"

namespace profit {
enum Placeability : bool {
  INVALID,
  VALID,
};
using PlacementMap = Field<Placeability, INVALID, INVALID>;

using geometry::Coordinate;

/* Assumes handle at top left */
template <const Vec2& ObjectDimensions>
inline PlacementMap placements_for(const OccupancyMap& occupancy_map,
                                   const DistanceMap& distance_map) {
  PlacementMap placements(occupancy_map.dimensions());
  Coordinate last_invalid = -1;

  for (auto y = 0; y < placements.dimensions().y(); ++y) {
    for (Coordinate i = 0 - ObjectDimensions.width(), j = -1; i < placements.dimensions().x();
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
    for (Coordinate i = 0 - ObjectDimensions.height(), j = -1; i < placements.dimensions().y();
         ++i, ++j) {
      if (placements.at(Vec2{x, j}) == INVALID) {
        last_invalid = j;
      }
      if (last_invalid >= i && i >= 0) {
        placements.set(Vec2{x, i}, INVALID);
      }
    }
  }

  return placements;
}

}  // namespace profit
