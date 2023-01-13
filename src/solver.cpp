#include "solver.hpp"

#include "objects/placeable.hpp"
#include "score.hpp"

namespace profit {
inline void solve_component(const ConnectedComponent& component, FieldState* state,
                            const profit::parsing::Input& input, const DistanceMap& merged) {
  std::cerr << "Starting with component\n";
  AvailableResources resources = available_resources(component, input);
  std::vector<ProductCount> fabrication_plan = pech(resources, input.products);
  std::cerr << " ----- Fabrication plan -----\n";
  for (auto product : fabrication_plan) {
    std::cerr << product << "\n";
  }
  std::cerr << " ----- Fabrication plan -----\n";
  // try to realize one by one
  // TODO: optimize by building product with higher scores first see #24
  // TODO: optimize by building the correct proportions see #25
  for (unsigned int i = 0; i < fabrication_plan.size(); i++) {
    auto product = input.products[i];
    auto count = fabrication_plan[i];
    std::cerr << "Starting with product " << i << ", count is " << count << "\n";
    if (count == 0) {
      continue;
    }
    std::cerr << "Trying to place factory\n";
    auto factory_id = place_factory(input.products[i].type, merged, state);
    if (!factory_id) {
      continue;
    }
    std::cerr << "Placed factory\n";
    for (auto resource_type : RESOURCE_TYPES) {
      if (product.requirements[resource_type] == 0) {
        continue;
      }
      for (auto deposit_id : component) {
        auto deposit = input.deposits[deposit_id];
        if (deposit.type != resource_type) {
          continue;
        }
        auto pipeline_id = connect(deposit, *factory_id, state);
        if (!pipeline_id) {
          continue;
        }
      }
    }
  }
}

void simple_greedy_solver(const parsing::Input& input,
                          const std::function<void(const Solution)>& update_solution) {
  OccupancyMap occupancy_map = occupancies_from(input);
  auto deposits = input.deposits;
  ConnectedComponentsWrapper components_wrapper(static_cast<DepositId>(deposits.size()),
                                                occupancy_map.dimensions());

  std::vector<ConnectedComponent> connected_components = components_wrapper.extract();
  std::vector<DistanceMap> distance_maps;
  distance_maps.reserve(deposits.size());
  FieldState state = {occupancy_map, {}, {}};

  for (size_t i = 0; i < deposits.size(); i++) {
    distance_maps.emplace_back(
        distances_from(deposits[i], occupancy_map, &components_wrapper, static_cast<DepositId>(i)));
  }

  const DistanceMap merged = merge(distance_maps, Factory::DIMENSIONS);

  for (const auto& component : connected_components) {
    solve_component(component, &state, input, merged);
    auto solution_score = score(state, input.turns, input);
    update_solution(Solution{solution_score, state.placed_objects()});
  }
}
}  // namespace profit
