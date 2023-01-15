#include "miscellany/connected_component.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "objects/placeable.hpp"
#include "resources/example_tasks.hpp"

using namespace geometry;
using namespace profit;

class ConnectedComponentTest : public testing::Test {
 public:
  static void with_example(const std::string_view& example,
                           const std::vector<ConnectedComponent>& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);

    OccupancyMap occupancy_map = occupancies_from(input);
    auto deposits = input.deposits;
    ConnectedComponentsWrapper components_wrapper(static_cast<DepositId>(deposits.size()),
                                                  occupancy_map.dimensions());
    for (size_t i = 0; i < deposits.size(); i++) {
      distances_from(deposits[i], occupancy_map, &components_wrapper, static_cast<DepositId>(i));
    }

    std::vector<ConnectedComponent> actual = components_wrapper.extract();

    EXPECT_EQ(actual, expected);
  }
};

TEST(ConnectedComponent, Task1) {
  ConnectedComponentTest::with_example(examples::TASK1, {{0, 1, 2}});
}

TEST(ConnectedComponent, Task2) { ConnectedComponentTest::with_example(examples::TASK2, {{0}}); }

TEST(ConnectedComponent, Task3) { ConnectedComponentTest::with_example(examples::TASK3, {{0, 1}}); }

TEST(ConnectedComponent, Task4) {
  ConnectedComponentTest::with_example(examples::TASK4, {{0, 1, 2, 3}});
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
                                       {{0, 1}, {2, 3}, {4, 5}, {6, 7}});
}
