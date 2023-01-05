#include "fields/occupancy_map.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

#include "example_tasks.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "rotation.hpp"

using namespace geometry;
using namespace profit;
using namespace parsing;

// NOLINTBEGIN(readability-identifier-length): Keep Visualization of expected fields comprehensible
const CellOccupancy o = CellOccupancy::EMPTY;
const CellOccupancy x = CellOccupancy::BLOCKED;
const CellOccupancy e = CellOccupancy::EGRESS;
// NOLINTEND(readability-identifier-length)

class OccupancyMapTest : public testing::Test {
 public:
  static void with_example(const std::string_view& example,
                           const std::vector<CellOccupancy>& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);
    OccupancyMap occupancy_map = occupancies_from(input);
    EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
  }

  static void with_objects(Vec2 dimensions, const std::vector<Deposit>& deposits,
                           const std::vector<Obstacle>& obstacles,
                           const std::vector<CellOccupancy>& expected) {
    auto input = Input{.dimensions = dimensions,
                       .turns = 0,
                       .time = 0,
                       .products = {},
                       .deposits = deposits,
                       .obstacles = obstacles};
    OccupancyMap occupancy_map = occupancies_from(input);
    EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
  }
};

TEST(OccupancyMap, Obstacle) {
  OccupancyMapTest::with_objects(
      Vec2{5, 5}, {},
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
      {},
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
