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

PipelineId connect(const Deposit /*deposit*/, const Factory /*factory*/,
                   FieldState* /*field_state*/) {
  return INVALID_PIPELINE_ID;
}

}  // namespace profit
