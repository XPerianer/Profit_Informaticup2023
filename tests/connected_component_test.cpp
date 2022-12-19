#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "example_tasks.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"

using namespace geometry;
using namespace profit;

class ConnectedComponentTest : public testing::Test {
 public:
  static void with_example(std::string_view example,
                           const std::vector<ConnectedComponent>&& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);

    OccupancyMap occupancy_map = occupancies_from(input);
    auto deposits = get_deposits(input);

    std::vector<std::vector<Vec2>> reached_egresses = {};
    for (size_t i = 0; i < deposits.size(); i++) {
      reached_egresses.emplace_back();
      distances_from(deposits[i], occupancy_map, reached_egresses[i]);
    }

    std::vector<ConnectedComponent> actual = connected_components_from(deposits, reached_egresses);

    EXPECT_EQ(actual, expected);
  }
};

TEST(ConnectedComponent, Task1) {
  ConnectedComponentTest::with_example(examples::TASK1, {{2, 1, 0}});
}

TEST(ConnectedComponent, Task2) { ConnectedComponentTest::with_example(examples::TASK2, {{0}}); }

TEST(ConnectedComponent, Task3) { ConnectedComponentTest::with_example(examples::TASK3, {{1, 0}}); }

TEST(ConnectedComponent, Task4) {
  ConnectedComponentTest::with_example(examples::TASK4, {{3, 2, 1, 0}});
}

TEST(ConnectedComponent, TrickyConveyorConnection) {
  ConnectedComponentTest::with_example(examples::TRICKY_CONVEYOR_CONNECTION, {{0}});
}

TEST(ConnectedComponent, VeryStrongContained) {
  ConnectedComponentTest::with_example(examples::VERY_STRONG_CONSTRAINED, {{0}});
}

TEST(ConnectedComponent, VerticallySplitMap) {
  ConnectedComponentTest::with_example(examples::VERTICALLY_SPLIT_MAP, {{1}, {0}});
}

TEST(ConnectedComponent, MultipleConnectedComponents) {
  ConnectedComponentTest::with_example(examples::MULTIPLE_CONNECTED_COMPONENTS,
                                       {{7, 6}, {5, 4}, {3, 2}, {1, 0}});
}
