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

[[nodiscard]] inline Solution merge(const std::vector<Solution>& solutions) {
  profit::ProductScore score = 0;
  std::vector<PlaceableObject> parts;
  for (auto solution : solutions) {
    score += solution.score;
    parts.insert(parts.end(), solution.parts.begin(), solution.parts.end());
  }
  return Solution{score, parts};
}

void simple_greedy_solver(const parsing::Input& input,
                          const std::function<void(const Solution)>& update_solution);

}  // namespace profit
