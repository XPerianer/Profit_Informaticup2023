#include <algorithm>
#include <iostream>
#include <variant>
#include <vector>

#include "io/parsing.hpp"
#include "io/serialization.hpp"
#include "solver.hpp"

// NOLINTBEGIN(google-build-using-namespace): In main, we allow using these
using namespace profit;
using namespace geometry;
// NOLINTEND(google-build-using-namespace)

// For now, we allow exception escape (causing std::terminate -> error shown)
int main() {  // NOLINT(bugprone-exception-escape)
  parsing::Input input = parsing::parse(std::cin);

  std::vector<PlaceableObject> result = solver::simple_greedy_solver(input);

#ifdef NDEBUG
  std::cout << serialization::serialize(result);
#else
  // Extented output for profit website
  serialization::Output output =
      serialization::Output{input.dimensions, input.turns,     input.time, input.products,
                            input.deposits,   input.obstacles, result};
  std::cout << serialization::serialize_detailed(output);
#endif
  return 0;
}
