#pragma once
#include <concepts>
#include <vector>

#include "connect.hpp"
#include "io/parsing.hpp"
#include "mdkp.hpp"
#include "placeable.hpp"
#include "resource_type.hpp"

namespace profit {

struct Solution {
  profit::ProductScore score = 0;
  std::vector<PlaceableObject> parts;
};

void simple_greedy_solver(const parsing::Input& input,
                          std::function<void(Solution)> update_solution);

}  // namespace profit
