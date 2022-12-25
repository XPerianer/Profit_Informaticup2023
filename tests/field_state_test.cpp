#include "field_state.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "connected_component.hpp"
#include "example_tasks.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "product.hpp"

using namespace geometry;
using namespace profit;
using namespace parsing;

class FieldStateTest : public testing::Test {
 public:
  static void should_be_reachable_in(const std::string_view& example,
                                     const std::vector<DistanceT>& expected_factory_distances) {
    std::istringstream stream{std::string{example}};
    Input input = parsing::parse(stream);
    OccupancyMap occupancy_map = occupancies_from(input);
    std::vector<Deposit> deposits = get_deposits(input);
    ConnectedComponentUnion cc_union(static_cast<DepositId>(deposits.size()),
                                     occupancy_map.dimensions());
    std::vector<DistanceMap> distance_maps;
    distance_maps.reserve(deposits.size());

    for (size_t i = 0; i < deposits.size(); i++) {
      distance_maps.emplace_back(
          distances_from(deposits[i], occupancy_map, cc_union, static_cast<DepositId>(i)));
    }

    FieldState state = {occupancy_map, {}, {}};
    FactoryId factory = place_factory(input.products[0].type, distance_maps, state).value_or(-1);
    std::array<Vec2, Factory::OCCUPIED_CELL_COUNT> occupied =
        state.factories[factory].occupied_cells();

    for (size_t i = 0; i < deposits.size(); i++) {
      Vec2 min_coordinate = *std::ranges::min_element(occupied, [&](auto cell_a, auto cell_b) {
        return distance_maps[i].at(cell_a) < distance_maps[i].at(cell_b);
      });
      EXPECT_EQ(distance_maps[i].at(min_coordinate), expected_factory_distances[i]);
    }
  }
};

TEST(FieldState, NoPlacementPossible) {
  OccupancyMap occupancy_map(Vec2{4, 4});
  FieldState state{occupancy_map, {}, {}};
  ProductType type{};
  std::vector<DistanceMap> distance_maps = {DistanceMap(Vec2{4, 4})};
  EXPECT_FALSE(place_factory(type, distance_maps, state).has_value());
}

TEST(FieldState, Task1) { FieldStateTest::should_be_reachable_in(examples::TASK1, {1, 1, 5}); }

TEST(FieldState, Task2) { FieldStateTest::should_be_reachable_in(examples::TASK2, {4}); }

TEST(FieldState, Task3) { FieldStateTest::should_be_reachable_in(examples::TASK3, {6, 6}); }

TEST(FieldState, Task4) { FieldStateTest::should_be_reachable_in(examples::TASK4, {4, 4, 4, 4}); }

TEST(FieldState, VeryStrongConstrained) {
  FieldStateTest::should_be_reachable_in(examples::VERY_STRONG_CONSTRAINED, {1});
}

TEST(FieldState, TrickyConveyorConnection) {
  FieldStateTest::should_be_reachable_in(examples::TRICKY_CONVEYOR_CONNECTION, {8});
}
