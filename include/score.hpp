#pragma once

#include <sys/types.h>

#include "field_state.hpp"
#include "io/parsing.hpp"
#include "mdkp.hpp"
#include "objects/landscape.hpp"
#include "objects/product.hpp"
using Turn = uint32_t;

const profit::ResourceRequirement flow_per_time_step = 3;

[[nodiscard]] inline std::vector<Turn> when_are_deposits_empty(
    const profit::FieldState& state, const profit::parsing::Input& input) {
  std::vector<Turn> when_empty(input.deposits.size());
  std::vector<int> connected_pipelines(input.deposits.size());
  for (const auto& [_, pipeline] : state.pipelines) {
    connected_pipelines[pipeline.deposit_id]++;
  }
  for (unsigned int i = 0; i < input.deposits.size(); i++) {
    const auto& deposit = input.deposits[i];
    when_empty[i] = static_cast<Turn>(profit::initial_resource_count(deposit)) /
                    std::max(static_cast<profit::ResourceRequirement>(1),
                             flow_per_time_step * connected_pipelines[i]);
  }
  return when_empty;
}

[[nodiscard]] inline profit::ResourceRequirement ressources_at_turn(const auto& pipeline,
                                                                    Turn empty_at, Turn turn) {
  if (pipeline.parts.size() + 1 >= turn) {
    return 0;
  }
  return std::min(flow_per_time_step * (turn - pipeline.parts.size() - 1),
                  static_cast<profit::ResourceRequirement>(empty_at * flow_per_time_step));
}

[[nodiscard]] inline profit::ProductScore score(const profit::FieldState& state, Turn turn,
                                                const profit::parsing::Input& input) {
  profit::ProductScore score = 0;
  std::vector<Turn> when_empty = when_are_deposits_empty(state, input);
  std::vector<profit::AvailableResources> resources_per_factory(state.factories.size());
  for (const auto& [pipeline_id, pipeline] : state.pipelines) {
    auto deposit = input.deposits[pipeline.deposit_id];
    auto product_count = ressources_at_turn(pipeline, when_empty[pipeline.deposit_id], turn);

    resources_per_factory[pipeline.factory_id][static_cast<char>(deposit.type)] += product_count;
  }

  for (const auto& [factory_id, factory] : state.factories) {
    auto available_resources = resources_per_factory[factory_id];

    score +=
        profit::product_score(available_resources, input.products[static_cast<char>(factory.type)]);
  }

  return score;
}
