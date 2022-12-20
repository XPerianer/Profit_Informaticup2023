#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <queue>
#include <ranges>
#include <vector>

#include "example_tasks.hpp"
#include "fields/connected_component.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "landscape.hpp"
#include "placeable.hpp"
#include "rotation.hpp"

using namespace geometry;
using namespace profit;
using namespace parsing;

// NOLINTBEGIN(readability-identifier-length): Keep Visualization of expected fields comprehensible
const CellOccupancy o = CellOccupancy::EMPTY;
const CellOccupancy x = CellOccupancy::BLOCKED;
const CellOccupancy e = CellOccupancy::EGRESS;
const int n = static_cast<int>(NOT_REACHABLE);
// NOLINTEND(readability-identifier-length)

class OccupancyMapTest : public testing::Test {
 public:
  static void with_example(std::string_view example, const std::vector<CellOccupancy>& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);
    OccupancyMap occupancy_map = occupancies_from(input);
    EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
  }

  static void with_objects(Vec2 dimensions, const std::vector<LandscapeObject>& objects,
                           const std::vector<CellOccupancy>& expected) {
    auto input =
        Input{.dimensions = dimensions, .turns = 0, .time = 0, .products = {}, .objects = objects};
    OccupancyMap occupancy_map = occupancies_from(input);
    EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
  }
};

TEST(OccupancyMap, Obstacle) {
  OccupancyMapTest::with_objects(
      Vec2{5, 5},
      {
          Obstacle{.handle = {2, 2}, .dimensions = {2, 2}},
      },
      {
          // clang-format off
      o, o, o, o, o,
      o, o, o, o, o,
      o, o, x, x, o,
      o, o, x, x, o,
      o, o, o, o, o,
          // clang-format on
      });
}

TEST(OccupancyMap, Deposit) {
  OccupancyMapTest::with_objects(
      Vec2{5, 5},
      {
          Deposit{.handle = {2, 2}, .dimensions = {3, 3}},
      },
      {
          // clang-format off
      o, o, o, o, o,
      o, o, o, o, o,
      o, o, e, e, e,
      o, o, e, x, e,
      o, o, e, e, e,
          // clang-format on
      });
}

TEST(OccupancyMap, Task1) {
  OccupancyMapTest::with_example(
      examples::TASK1,
      {
          // clang-format off
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, 
          o, e, e, e, e, e, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, e, e, e, e, e, e, o,
          o, e, x, x, x, e, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, x, x, x, x, x, e, o,
          o, e, x, x, x, e, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, x, x, x, x, x, e, o,
          o, e, x, x, x, e, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, x, x, x, x, x, e, o,
          o, e, e, e, e, e, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, x, x, x, x, x, e, o,
          o, o, o, o, o, o, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, x, x, x, x, x, e, o,
          o, o, o, o, o, o, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, e, e, e, e, e, e, e, o,
          o, o, o, o, o, o, o, o, o, o, o, x, x, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x,
          o, o, o, o, o, o, o, o, o, o, o, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          // clang-format on
      });
}

TEST(OccupancyMap, Task2) {
  OccupancyMapTest::with_example(
      examples::TASK2,
      {
          // clang-format off
      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, x, x, x, e, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, o, o, o, o, o,
      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          // clang-format on
      });
}

TEST(OccupancyMap, Task3) {
  OccupancyMapTest::with_example(
      examples::TASK3,
      {
          // clang-format off
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, 
          o, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, x, x, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, e, e, e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, e, e, e, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, e, x, e, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, e, e, e, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          // clang-format on
      });
}

TEST(OccupancyMap, Task4) {
  OccupancyMapTest::with_example(
      examples::TASK4,
      {
          // clang-format off
          e, e, e, e, e, e, e, e, x, x, x, x, x, x, x, x, x, x, x, x, x, e, e, e, e, e, e, e, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, e, e, e, e, e, e, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, e, e, e, e, e, e, e,
          o, o, o, o, o, o, o, o, x, x, x, x, o, o, o, o, o, x, x, x, x, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, x, x, x, x, o, o, x, o, o, x, x, x, x, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, o, o, o, o, o, o, x, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, x, x, x, x, o, o, x, o, o, x, x, x, x, o, o, o, o, o, o, o, o,
          o, o, o, o, o, o, o, o, x, x, x, x, o, o, o, o, o, x, x, x, x, o, o, o, o, o, o, o, o,
          e, e, e, e, e, e, e, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, e, e, e, e, e, e, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, x, x, x, x, x, x, e, x, x, x, x, o, o, o, o, o, x, x, x, x, e, x, x, x, x, x, x, e,
          e, e, e, e, e, e, e, e, x, x, x, x, x, x, x, x, x, x, x, x, x, e, e, e, e, e, e, e, e,
          // clang-format on
      });
}

TEST(OccupancyMap, NoCollision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_FALSE(collides(object, occupancy_map));
}

TEST(OccupancyMap, Collision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::BLOCKED);
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides(object, occupancy_map));
}

TEST(OccupancyMap, CollisionWithConveyor) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::CONVEYOR_CROSSING);
  auto conveyor = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  auto combiner = Combiner{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides(combiner, occupancy_map));
  EXPECT_FALSE(collides(conveyor, occupancy_map));
}

class DistanceMapTest : public testing::Test {
 public:
  static void with_example(std::string_view example,
                           const std::vector<std::vector<DistanceT>>& expected_distances) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);
    OccupancyMap occupancy_map = occupancies_from(input);
    std::vector<Deposit> deposits = get_deposits(input);
    ConnectedComponentUnion cc_union(static_cast<DepositId>(deposits.size()),
                                     occupancy_map.dimensions());

    for (size_t i = 0; i < deposits.size(); i++) {
      DistanceMap distance_map =
          distances_from(deposits[i], occupancy_map, cc_union, static_cast<DepositId>(i));
      EXPECT_THAT(distance_map.map(), testing::ElementsAreArray(expected_distances[i]));
    }
  }
};

TEST(DistanceMap, Task1) {
  DistanceMapTest::with_example(
      examples::TASK1,
      {{
           // clang-format off
          n, n, n, n, n, n, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 2, 2, 1, 1, n, n, 3, 3, 3, 3, 4, 4, 4, 4, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 2, 2, 1, 1, n, n, 3, 3, 3, 4, 4, 4, 4, 5, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 2, 2, 1, 1, n, n, 3, 3, 4, 4, 4, 4, 5, 5, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 1, 2, 1, 1, n, n, 3, 4, 4, 4, 4, 5, 5, 5, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 1, 1, 1, 1, n, n, 4, 4, 4, 4, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, 
          1, n, n, n, n, n, 1, 1, 2, 1, 2, n, n, 4, 4, 4, 5, 5, 5, 5, 6, n, n, n, n, n, n, n, n, n, 
          1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, n, n, 4, 4, 5, 5, 5, 5, 6, 6, n, n, n, n, n, n, n, n, n, 
          2, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, n, n, 4, 5, 5, 5, 5, 6, 6, 6, 6, n, n, n, n, n, n, n, n, 
          1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 
          2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 
          2, n, n, n, n, n, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 
          n, n, n, n, n, n, n, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 
          n, n, n, n, n, n, n, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 
          n, n, n, n, n, n, n, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 
          n, n, n, n, n, n, n, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 
          n, n, n, n, n, n, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 4, 4, 4, 5, n, n, 6, 6, 6, 7, 7, 7, 7, 8, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 4, 4, 4, 4, n, n, 7, 7, 7, 7, 7, 7, 8, 8, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 3, 4, 4, 4, n, n, 7, 7, 7, 7, 7, 8, 8, 8, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 3, 3, 4, 4, n, n, 7, 7, 7, 7, 8, 8, 8, 8, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 3, 3, 3, 4, n, n, 7, 7, 7, 8, 8, 8, 8, 9, n, n, n, n, n, n, n, n, n, 
          2, n, n, n, n, n, 2, 3, 3, 3, 3, n, n, 8, 8, 8, 8, 8, 8, 9, 9, n, n, n, n, n, n, n, n, n, 
          2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, n, n, 8, 8, 8, 8, 8, 9, 9, 9, n, n, n, n, n, n, n, n, n, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, n, n, 8, 8, 8, 8, 9, 9, 9, 9, 10, n, n, n, n, n, n, n, n, 
          2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          2, 2, 2, 2, 2, 1, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 
          2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 
          2, n, n, n, n, n, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 
          n, n, n, n, n, n, n, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 
          n, n, n, n, n, n, n, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 
          n, n, n, n, n, n, n, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 
          n, n, n, n, n, n, n, 1, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 
          n, n, n, n, n, n, n, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 
          n, n, n, n, n, n, 1, 1, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 2, 1, 1, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 4, 4, 4, 4, n, n, 2, 2, 2, 2, 1, 1, 1, 1, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 5, 4, 4, 4, n, n, 2, 2, 2, 2, 1, 1, 2, 1, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 5, 5, 4, 4, n, n, 2, 2, 2, 2, 1, 1, 2, 1, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 5, 5, 5, 4, n, n, 2, 2, 2, 2, 1, 1, 2, 1, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, 5, 5, 5, 5, n, n, 2, 2, 2, 2, 1, 1, 2, 2, n, n, n, n, n, n, n, n, n, 
          8, n, n, n, n, n, 6, 6, 5, 5, 5, n, n, 2, 2, 2, 2, 1, 1, 2, 2, n, n, n, n, n, n, n, n, n, 
          8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, n, n, 2, 2, 2, 2, 1, 1, 2, 2, n, n, n, n, n, n, n, n, n, 
          8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, n, n, 3, 2, 2, 2, 2, 1, 2, 2, 2, n, n, n, n, n, n, n, n, 
          8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 
          9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 
          9, n, n, n, n, n, 8, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 
          n, n, n, n, n, n, n, 8, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 
          n, n, n, n, n, n, n, 8, 8, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 
          n, n, n, n, n, n, n, 8, 8, 8, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 
          n, n, n, n, n, n, n, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 
          n, n, n, n, n, n, n, 9, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 
          n, n, n, n, n, n, 9, 9, 9, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13,
           // clang-format on
       }});
}

TEST(DistanceMap, Task2) {
  DistanceMapTest::with_example(
      examples::TASK2,
      {{
          // clang-format off
      n, n, n, n, n, n, 2, 2, 1, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6,
      n, n, n, n, n, n, 2, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 
      n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 6,
      n, n, n, n, n, n, 2, 2, 1, 3, 3, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6,
      n, n, n, n, n, n, 2, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
          // clang-format on
      }});
}

TEST(DistanceMap, Task3) {
  DistanceMapTest::with_example(
      examples::TASK3,
      {{
           // clang-format off
          n, n, n, n, n, n, n, n, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 2, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 1, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 1, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 1, 2, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 
          1, n, n, n, n, n, n, n, 1, 1, 2, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 
          1, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 
          2, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 
          1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 
          2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 
          3, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 
          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 
          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 
          3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 
          4, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 
          4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 
          4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 
          4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 
          5, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 
          5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 
          5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 
          6, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 
          6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 
          6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 
          6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 
          7, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 
          7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 
          7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 
          7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, n, n, n, 15, 
          8, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, n, n, n, n, n, 
          8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, n, n, n, n, n, 
          8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, n, n, n, n, n, 
          8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, n, n, n, 16,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, n, n, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9, 
          n, n, n, n, n, n, n, n, n, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 
          n, n, n, n, n, n, n, n, n, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 
          n, n, n, n, n, n, n, n, n, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 9, 
          n, n, n, n, n, n, n, n, n, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 
          n, n, n, n, n, n, n, n, n, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 8, 
          16, n, n, n, n, n, n, n, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 
          16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 
          16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 
          15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 7, 
          15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 
          15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 
          15, 14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 
          14, 14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 6, 
          14, 14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 
          14, 14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 
          14, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 
          13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 5, 
          13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 
          13, 13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 
          13, 12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 
          12, 12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 4, 
          12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 
          12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 
          12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 
          11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 3, 
          11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 
          11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 
          11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 
          10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 2, 
          10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 
          10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 2, 1, 2, 
          10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 
          9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 2, 1, 2, n, n, n, 2, 
          9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 2, n, n, n, n, n, 
          9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 2, 2, n, n, n, n, n, 
          9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 2, 2, n, n, n, n, n, 
          9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 2, 2, 2, n, n, n, 3,
           // clang-format on
       }});
}

TEST(DistanceMap, Task4) {
  DistanceMapTest::with_example(
      examples::TASK4,
      {{
           // clang-format off
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 7, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 4, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          1, 1, 1, 1, 1, 2, 2, 2, n, n, n, n, 3, 4, n, 5, 5, n, n, n, n, 7, 7, 7, 8, 8, 8, 8, 9, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, n, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 
          1, 1, 1, 1, 1, 1, 1, 1, n, n, n, n, 3, 4, n, 5, 5, n, n, n, n, 7, 7, 7, 8, 8, 8, 8, 9, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 4, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 7, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 7, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 4, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          1, 1, 1, 1, 1, 1, 1, 1, n, n, n, n, 3, 4, n, 5, 5, n, n, n, n, 7, 7, 7, 8, 8, 8, 8, 9, 
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, n, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 
          2, 2, 2, 1, 1, 1, 1, 1, n, n, n, n, 3, 4, n, 5, 5, n, n, n, n, 7, 7, 7, 8, 8, 8, 8, 9, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 4, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 4, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 4, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 4, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 7, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 7, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 4, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          9, 8, 8, 8, 8, 7, 7, 7, n, n, n, n, 5, 5, n, 4, 3, n, n, n, n, 1, 1, 1, 1, 1, 2, 2, 2, 
          8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, n, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
          9, 8, 8, 8, 8, 7, 7, 7, n, n, n, n, 5, 5, n, 4, 3, n, n, n, n, 1, 1, 1, 1, 1, 1, 1, 1, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 4, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 7, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
           // clang-format on
       },
       {
           // clang-format off
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 7, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 4, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          9, 8, 8, 8, 8, 7, 7, 7, n, n, n, n, 5, 5, n, 4, 3, n, n, n, n, 1, 1, 1, 1, 1, 1, 1, 1, 
          8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, n, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
          9, 8, 8, 8, 8, 7, 7, 7, n, n, n, n, 5, 5, n, 4, 3, n, n, n, n, 2, 2, 2, 1, 1, 1, 1, 1, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 4, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 4, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 4, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 5, 5, 5, 5, 4, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 5, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 5, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 5, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 6, 6, 6, 6, 5, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, 7, 6, 6, 6, 6, n, n, n, n, n, n, n, n, n, n, n, n, 
          n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n,
           // clang-format on
       }});
}
