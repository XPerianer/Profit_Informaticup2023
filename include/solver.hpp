#pragma once
#include <concepts>
#include <vector>

#include "connect.hpp"
#include "io/parsing.hpp"
#include "mdkp.hpp"
#include "objects/placeable.hpp"
#include "objects/resource_type.hpp"

namespace profit {

struct Solution {
  profit::ProductScore score = 0;
  std::vector<PlaceableObject> parts;
};

void simple_greedy_solver(const parsing::Input& input,
                          const std::function<void(const Solution)>& update_solution);

}  // namespace profit
