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

// playing field is at max 100*100 big, so 16 bytes are enough to address every field with a single
// index
using FieldIndex = uint16_t;
const auto INVALID_FIELD = std::numeric_limits<FieldIndex>::max();

PipelineId connect(const Deposit /*deposit*/, const FactoryId factory_id, FieldState* state) {
  Field<FieldIndex, INVALID_FIELD, INVALID_FIELD> predecessors(state->occupancy_map.dimensions());

  // Step 0: check which fields we can reach: this can be either existing pipeline or factory
  // borders
  Field<bool, false, false> target_ingresses(predecessors.dimensions());

  for (const auto& [id, pipeline] : state->pipelines) {
    if (id == factory_id) {
      for (const auto& placeable : pipeline.parts) {
        std::visit(utils::Overloaded{
                       [](Mine& mine) { target_ingresses[mine.ingress()] = true; },
                       [](Combiner& combiner) {
                         for (const auto& ingress : combiner.ingresses()) {
                           target_ingresses[ingress] = true;
                         }
                       },
                       [](Conveyor3 conveyor) { target_ingresses[conveyor.ingress()] = true; },
                       [](Conveyor4 conveyor) { target_ingresses[conveyor.ingress()] = true; }},
                   placeable);
      };
    }
  }
  // Step 1: Go through all mines and add possible ingresses in queue
  // Step 2: Loop over all
  // ingresses in queue and add possible ingresses based on the objects we can potentially place,
  // terminating when we have reached desired factory field
  // Step 3: materialize by tracing back the
  // predecessors of the fields we have written and placing these objects in the placeable parts
  // Step 4: update field parts? Who should do this?
  // TODO: Potential problem: Dijkstra could self-intersect

  return INVALID_PIPELINE_ID;
}

}  // namespace profit
