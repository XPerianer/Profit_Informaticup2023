#pragma once

#include "fields/distance_map.hpp"
#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/coordinate.hpp"
#include "geometry/vec2.hpp"
#include "landscape.hpp"

namespace profit {
enum Placeability : bool {
  IMPOSSIBLE,
  POSSIBLE,
};
using PlacementMap = Field<Placeability, IMPOSSIBLE, IMPOSSIBLE>;

using geometry::Coordinate;

/* Assumens handle at top left */
inline PlacementMap placements_for(const Vec2 object_dimensions, const OccupancyMap& occupancy_map,
                                   const DistanceMap& distance_map) {
  PlacementMap placements(occupancy_map.dimensions());
  Coordinate last_unsuitable = -1;

  for (auto y = 0; y < placements.dimensions().y(); ++y) {
    for (Coordinate i = 0 - object_dimensions.x(), j = -1; i < placements.dimensions().x();
         ++i, ++j) {
      if (occupancy_map.at(Vec2{j, y}) != CellOccupancy::EMPTY ||
          distance_map.at(Vec2{j, y}) == NOT_REACHABLE) {
        last_unsuitable = j;
      }
      if (last_unsuitable < i) {
        placements.set(Vec2{i, y}, POSSIBLE);
      }
    }
  }

  for (auto x = 0; x < placements.dimensions().x(); ++x) {
    for (Coordinate i = 0 - object_dimensions.y(), j = -1; i < placements.dimensions().y();
         ++i, ++j) {
      if (placements.at(Vec2{x, j}) == IMPOSSIBLE) {
        last_unsuitable = j;
      }
      if (last_unsuitable >= i && i >= 0) {
        placements.set(Vec2{x, i}, IMPOSSIBLE);
        continue;
      }
    }
  }

  for (Vec2 cell : placements) {
    if (cell.x() == 0) {
      std::cout << std::endl;
    }
    if (placements.at(cell) == IMPOSSIBLE) {
      std::cout << "n";
    } else {
      std::cout << "j";
    }
  }
  return placements;
}

}  // namespace profit