#pragma once
#include <concepts>
#include <vector>

#include "connect.hpp"
#include "io/parsing.hpp"
#include "mdkp.hpp"
#include "placeable.hpp"
#include "resource_type.hpp"

namespace profit::solver {

std::vector<profit::PlaceableObject> simple_greedy_solver(profit::parsing::Input& input);

}  // namespace profit::solver
