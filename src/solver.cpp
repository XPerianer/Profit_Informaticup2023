#include "solver.hpp"

#include "fields/component_map.hpp"
#include "fields/distance_map.hpp"
#include "objects/placeable.hpp"
#include "score.hpp"

namespace profit {

inline std::optional<FactoryId> get_realizing_factory(ProductType type, const FieldState& state) {
  for (auto [factory_id, factory] : state.factories) {
    if (factory.type == type) {
      return factory_id;
    }
  }
  return std::nullopt;
}

inline void try_placing_factories(FieldState* state, const profit::parsing::Input& input,
                                  const DistanceMap& merged,
                                  const std::vector<ProductCount>& fabrication_plan) {
  for (unsigned int i = 0; i < fabrication_plan.size(); i++) {
    auto product = input.products[i];
    auto count = fabrication_plan[i];
    DEBUG_PRINT("Starting with product " << i << ", count is " << count << "\n");
    if (count == 0) {
      continue;
    }
    auto factory_id = get_realizing_factory(product.type, *state);
    if (!factory_id) {
      DEBUG_PRINT("Trying to place factory\n");
      factory_id = place_factory(input.products[i].type, merged, state);
      if (!factory_id) {
        continue;
      }
    }
    DEBUG_PRINT("Placed factory\n");
  }
}

inline bool solve_component(const ConnectedComponent& component, FieldState* state,
                            const profit::parsing::Input& input, const DistanceMap& merged) {
  bool changed_something = false;

  DEBUG_PRINT("Starting with component with size " << component.size() << " \n");
  AvailableResources resources = available_resources(component, input);
  std::vector<ProductCount> fabrication_plan = pech(resources, input.products);
#ifndef NDEBUG
  DEBUG_PRINT(" ----- Fabrication plan -----\n");
  for (auto product : fabrication_plan) {
    DEBUG_PRINT(product << "\n");
  }
  DEBUG_PRINT(" ----- Fabrication plan -----\n");
#endif
  // TODO: optimize by building product with higher scores first see #24
  // TODO: optimize by building the correct proportions see #25
  try_placing_factories(state, input, merged, fabrication_plan);

  for (const auto& product : input.products) {
    for (auto resource_type : RESOURCE_TYPES) {
      DEBUG_PRINT("Starting with resource " << static_cast<int>(resource_type)
                                            << " requirements are "
                                            << product.requirements[resource_type] << "\n");

      if (product.requirements[resource_type] == 0) {
        continue;
      }
      auto factory_id = get_realizing_factory(product.type, *state);
      if (!factory_id) {
        continue;
      }
      for (auto deposit_id : component) {
        auto deposit = input.deposits[deposit_id];
        DEBUG_PRINT("Deposit type " << static_cast<int>(deposit.type) << "\n");
        if (deposit.type != resource_type) {
          continue;
        }
        DEBUG_PRINT("Connecting " << deposit_id << " with " << *factory_id << "\n");
        auto pipeline_id = connect(deposit_id, *factory_id, state, input);
        if (!pipeline_id) {
          DEBUG_PRINT("Failed connecting " << deposit_id << " with " << *factory_id << "\n");
          continue;
        }
        changed_something = true;
      }
    }
  }
  return changed_something;
}

void simple_greedy_solver(const parsing::Input& input,
                          const std::function<void(const Solution)>& update_solution) {
  OccupancyMap occupancy_map = occupancies_from(input);
  auto deposits = input.deposits;
  ConnectedComponentsWrapper components_wrapper(static_cast<DepositId>(deposits.size()),
                                                occupancy_map.dimensions());

  std::vector<DistanceMap> distance_maps;
  distance_maps.reserve(deposits.size());

  for (size_t i = 0; i < deposits.size(); i++) {
    distance_maps.emplace_back(
        distances_from(deposits[i], occupancy_map, &components_wrapper, static_cast<DepositId>(i)));
  }

  std::vector<ConnectedComponent> connected_components = components_wrapper.extract();
  std::vector<DistanceMap> cc_merged =
      merge_by_cc(connected_components, distance_maps, Factory::DIMENSIONS);
  std::vector<FieldState> field_states(connected_components.size(), {occupancy_map, {}, {}});
  std::vector<Solution> solutions(connected_components.size(), {0, {}});

  bool keep_running = true;
  while (keep_running) {
    keep_running = false;
    for (uint32_t i = 0; i < connected_components.size(); i++) {
      auto& component = connected_components[i];
      auto& state = field_states[i];
      DEBUG_PRINT("size: " << component.size() << "\n");

      keep_running |= solve_component(component, &state, input, cc_merged[i]);
      profit::ProductScore cap = std::max(solutions[i].score, score(state, input.turns, input));
      solutions[i] = Solution{cap, state.placed_objects()};
    }
    update_solution(merge(solutions));
  }
}
}  // namespace profit
