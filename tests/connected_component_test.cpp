#include "connected_component.hpp"

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
                           const std::vector<ConnectedComponent>& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);

    OccupancyMap occupancy_map = occupancies_from(input);
    auto deposits = input.deposits;
    ConnectedComponentUnion cc_union(static_cast<DepositId>(deposits.size()),
                                     occupancy_map.dimensions());
    for (size_t i = 0; i < deposits.size(); i++) {
      distances_from(deposits[i], occupancy_map, cc_union, static_cast<DepositId>(i));
    }

    std::vector<ConnectedComponent> actual = cc_union.extract();

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
  ConnectedComponentTest::with_example(examples::VERTICALLY_SPLIT_MAP, {{0}, {1}});
}

TEST(ConnectedComponent, MultipleConnectedComponents) {
  ConnectedComponentTest::with_example(examples::MULTIPLE_CONNECTED_COMPONENTS,
                                       {{1, 0}, {3, 2}, {5, 4}, {7, 6}});
}