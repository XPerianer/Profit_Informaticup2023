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

 protected:
  Solution best_solution_;
  std::function<void(const Solution)> update_solution_ = [&](const Solution& solution) {
    if (solution.score > best_solution_.score ||
        (solution.score == best_solution_.score &&
         solution.parts.size() > best_solution_.parts.size())) {
      best_solution_.score = solution.score;
      best_solution_.parts.assign(solution.parts.begin(), solution.parts.end());
    }
  };
};

TEST_F(SolverTest, Task1) {
  auto input = SolverTest::from_string(examples::TASK1);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 410);
  EXPECT_EQ(best_solution_.parts.size(), 16);
}

TEST_F(SolverTest, Task2) {
  auto input = SolverTest::from_string(examples::TASK2);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 90);
  EXPECT_EQ(best_solution_.parts.size(), 9);  // One factory and two pathes, each 4 long
}

TEST_F(SolverTest, Task3) {
  auto input = SolverTest::from_string(examples::TASK3);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 30);
  EXPECT_EQ(best_solution_.parts.size(), 38);
}

TEST_F(SolverTest, Task4) {
  auto input = SolverTest::from_string(examples::TASK4);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 0);
  EXPECT_EQ(best_solution_.parts.size(), 2);
}

TEST_F(SolverTest, ConstrainedToMineAndConveyor3) {
  auto input = SolverTest::from_string(examples::CONSTRAINED_TO_MINE_AND_CONVEYOR3);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 120);
  EXPECT_EQ(best_solution_.parts.size(), 3);
}

TEST_F(SolverTest, ConstrainedToMineAndConveyors) {
  auto input = SolverTest::from_string(examples::CONSTRAINED_TO_MINE_AND_CONVEYORS);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 120);
  EXPECT_EQ(best_solution_.parts.size(), 5);
}

TEST_F(SolverTest, ImpossibleDueToSelfIntersection) {
  auto input = SolverTest::from_string(examples::IMPOSSIBLE_DUE_TO_SELF_INTERSECTION);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 0);
  EXPECT_EQ(best_solution_.parts.size(), 1);
}

TEST_F(SolverTest, MultipleConnectedComponents) {
  auto input = SolverTest::from_string(examples::MULTIPLE_CONNECTED_COMPONENTS);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 40);
  EXPECT_EQ(best_solution_.parts.size(), 4);
}

TEST_F(SolverTest, TrickyConveyorConnection) {
  auto input = SolverTest::from_string(examples::TRICKY_CONVEYOR_CONNECTION);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 40);
  EXPECT_EQ(best_solution_.parts.size(), 9);
}

TEST_F(SolverTest, VerticallySplitMap) {
  auto input = SolverTest::from_string(examples::VERTICALLY_SPLIT_MAP);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 55);
  EXPECT_EQ(best_solution_.parts.size(), 10);
}

TEST_F(SolverTest, VeryStrongConstrained) {
  auto input = SolverTest::from_string(examples::VERY_STRONG_CONSTRAINED);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 290);
  EXPECT_EQ(best_solution_.parts.size(), 3);
}

TEST_F(SolverTest, DivideAndConquer) {
  auto input = SolverTest::from_string(examples::DIVIDE_AND_CONQUER);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 2160);
  EXPECT_EQ(best_solution_.parts.size(), 342);
}

TEST_F(SolverTest, Circle) {
  auto input = SolverTest::from_string(examples::CIRCLE);

  profit::simple_greedy_solver(input, update_solution_);

  EXPECT_EQ(best_solution_.score, 40);
  EXPECT_EQ(best_solution_.parts.size(), 83);
}