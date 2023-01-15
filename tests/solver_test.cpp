#include "solver.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include "connect.hpp"
#include "connected_component.hpp"
#include "field_state.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "miscellany/utils.hpp"
#include "objects/placeable.hpp"
#include "resources/example_tasks.hpp"

using namespace profit;

class SolverTest : public testing::Test {
 public:
  static parsing::Input from_string(const std::string_view& example) {
    std::stringstream stream{std::string{example}};
    return parsing::parse(stream);
  }
};

TEST(Solver, PlacesTwoMinesInTaskTwo) {
  auto input = SolverTest::from_string(examples::TASK2);

  Solution best_solution;
  profit::simple_greedy_solver(input, [&](const Solution& solution) {
    DEBUG("Update call with score " << solution.score << " and " << solution.parts.size() << "\n");
    if (solution.score > best_solution.score || best_solution.parts.empty()) {
      DEBUG("Best solution set new with score " << solution.score << " and "
                                                << solution.parts.size() << "\n");
      best_solution = solution;
    }
  });

  EXPECT_EQ(best_solution.score, 90);
  EXPECT_EQ(best_solution.parts.size(), 9);  // One factory and two pathes, each 4 long
}

TEST(Solver, BuildsManyPipelinesInTaskThree) {
  auto input = SolverTest::from_string(examples::TASK3);

  Solution best_solution;
  profit::simple_greedy_solver(input, [&](const Solution& solution) {
    DEBUG("Update call with score " << solution.score << " and " << solution.parts.size() << "\n");
    if (solution.score > best_solution.score || best_solution.parts.empty()) {
      DEBUG("Best solution set new with score " << solution.score << " and "
                                                << solution.parts.size() << "\n");
      best_solution = solution;
    }
  });

  EXPECT_EQ(best_solution.score, 10);
  EXPECT_EQ(best_solution.parts.size(), 38);
}
