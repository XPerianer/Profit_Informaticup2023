#include <gtest/gtest.h>

#include <ranges>
#include <vector>

#include "example_tasks.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "rotation.hpp"

using namespace geometry;
using namespace profit;

TEST(OccupancyMap, Obstacle) {
  auto input = parsing::Input{.dimensions = {5, 5},
                              .turns = 50,
                              .time = 300,
                              .products = {},
                              .objects = {
                                  Obstacle{.handle = {2, 2}, .dimensions = {2, 2}},
                              }};
  OccupancyMap occupancy_map = create_from(input);
  std::vector<CellOccupancy> expected = {
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY, CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY, CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY, CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
  };
  EXPECT_EQ(occupancy_map.map(), expected);
}

TEST(OccupancyMap, Deposit) {
  auto input = parsing::Input{.dimensions = {5, 5},
                              .turns = 50,
                              .time = 300,
                              .products = {},
                              .objects = {
                                  Deposit{.handle = {2, 2}, .dimensions = {3, 3}},
                              }};
  OccupancyMap occupancy_map = create_from(input);
  std::vector<CellOccupancy> expected = {
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,  CellOccupancy::EGRESS,
      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,  CellOccupancy::EGRESS,
      CellOccupancy::BLOCKED, CellOccupancy::EGRESS,

      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,  CellOccupancy::EGRESS,
      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,
  };
  EXPECT_EQ(occupancy_map.map(), expected);
}

TEST(OccupancyMap, Task2) {
  std::istringstream stream{std::string{examples::TASK2}};
  auto input = parsing::parse(stream);
  OccupancyMap occupancy_map = create_from(input);
  std::vector<CellOccupancy> expected = {
      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,  CellOccupancy::EGRESS,
      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EGRESS,  CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::EGRESS,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EGRESS,  CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::EGRESS,  CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EGRESS,  CellOccupancy::BLOCKED, CellOccupancy::BLOCKED,
      CellOccupancy::BLOCKED, CellOccupancy::EGRESS,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,  CellOccupancy::EGRESS,
      CellOccupancy::EGRESS,  CellOccupancy::EGRESS,  CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,   CellOccupancy::EMPTY,
      CellOccupancy::EMPTY,   CellOccupancy::EMPTY,

  };
  EXPECT_EQ(occupancy_map.map(), expected);
}

TEST(OccupancyMap, NoCollision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, true));
}

TEST(OccupancyMap, Collision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::BLOCKED);
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, true));
}

TEST(OccupancyMap, CollisionWithConveyor) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::CONVEYOR_CROSSING);
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, true));
}
