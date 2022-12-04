#include <gtest/gtest.h>

#include <queue>
#include <ranges>
#include <vector>

#include "example_tasks.hpp"
#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/vec2.hpp"
#include "io/parsing.hpp"
#include "placeable.hpp"
#include "rotation.hpp"

using namespace geometry;
using namespace profit;

std::vector<Vec2> queue_to_vector(std::queue<Vec2> queue) {
  std::vector<Vec2> result;
  while (!queue.empty()) {
    result.push_back(queue.front());
    queue.pop();
  }
  return result;
}

/**
 * 00000
 * 00000
 * 00xx0
 * 00xx0
 * 00000
 */
TEST(OccupancyMap, Obstacle) {
  auto input = parsing::Input{.dimensions = {5, 5},
                              .turns = 50,
                              .time = 300,
                              .products = {},
                              .objects = {
                                  Obstacle{.handle = {2, 2}, .dimensions = {2, 2}},
                              }};
  OccupancyMap occupancy_map = occupancies_from(input);
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

/**
 * 00000
 * 00000
 * 00eee
 * 00exe
 * 00eee
 */
TEST(OccupancyMap, Deposit) {
  auto input = parsing::Input{.dimensions = {5, 5},
                              .turns = 50,
                              .time = 300,
                              .products = {},
                              .objects = {
                                  Deposit{.handle = {2, 2}, .dimensions = {3, 3}},
                              }};
  OccupancyMap occupancy_map = occupancies_from(input);
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

/**
 * eeeee 00000 00000 00000 00000
 * exxxe 00000 00000 00000 00000
 * exxxe xxxxx xxxxx xxxxx 00000
 * exxxe 00000 00000 00000 00000
 * eeeee 00000 00000 00000 00000
 */
TEST(OccupancyMap, Task2) {
  std::istringstream stream{std::string{examples::TASK2}};
  auto input = parsing::parse(stream);
  OccupancyMap occupancy_map = occupancies_from(input);
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

/**
 * 00000
 * 0+o-0
 * 00000
 * 00000
 * 00000
 */
TEST(OccupancyMap, NoCollision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, true));
}

/**
 * 00000
 * 0x000
 * 00000
 * 00000
 * 00000
 */
TEST(OccupancyMap, Collision) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::BLOCKED);
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, true));
}

/**
 * 00000
 * 0o000
 * 00000
 * 00000
 * 00000
 */
TEST(OccupancyMap, CollisionWithConveyor) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{1, 1}, CellOccupancy::CONVEYOR_CROSSING);
  auto object = Conveyor3{.handle = Vec2{1, 1}, .rotation = Rotation::LEFT_TO_RIGHT};
  EXPECT_TRUE(collides<Conveyor3>(object, occupancy_map, false));
  EXPECT_FALSE(collides<Conveyor3>(object, occupancy_map, true));
}

/**
 * e//10
 * +//-1
 * 00010
 * 00000
 * 00000
 */
TEST(DistanceMap, FixedMinePlacement) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, CellOccupancy::EGRESS);
  std::queue<Vec2> reached_ingresses;
  place_objects(distance_map, occupancy_map,
                {Mine::with_ingress(Vec2{0, 1}, Rotation::LEFT_TO_RIGHT)}, 1, reached_ingresses);
  std::vector<Vec2> expected_reached_ingresses = {Vec2{3, 0}, Vec2{4, 1}, Vec2{3, 2}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(reached_ingresses);
  std::vector<DistanceT> expected_distances = {NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE};
  EXPECT_EQ(distance_map.map(), expected_distances);
  EXPECT_TRUE(std::is_permutation(expected_reached_ingresses.begin(),
                                  expected_reached_ingresses.end(),
                                  actual_reached_ingresses.begin()));
}

/**
 * e0010
 * 00001
 * 00010
 * 10100
 * 01000
 */
TEST(DistanceMap, MinePlacements) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, CellOccupancy::EGRESS);
  std::queue<Vec2> reached_ingresses;
  for (Vec2 possible_ingress_location : {Vec2{0, 1}, Vec2{1, 0}}) {
    for (auto rotation : ROTATIONS) {
      place_objects(distance_map, occupancy_map,
                    {Mine::with_ingress(possible_ingress_location, rotation)}, 1,
                    reached_ingresses);
    }
  }
  std::vector<Vec2> expected_reached_ingresses = {Vec2{3, 0}, Vec2{4, 1}, Vec2{3, 2},
                                                  Vec2{0, 3}, Vec2{2, 3}, Vec2{1, 4}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(reached_ingresses);
  std::vector<DistanceT> expected_distances = {NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,

                                               1,
                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE};
  EXPECT_EQ(distance_map.map(), expected_distances);
  EXPECT_TRUE(std::is_permutation(expected_reached_ingresses.begin(),
                                  expected_reached_ingresses.end(),
                                  actual_reached_ingresses.begin()));
}

/**
 * e0000
 * 02200
 * 22220
 * 00022
 * 01022
 */
TEST(DistanceMap, MoveByOne) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, CellOccupancy::EGRESS);
  std::queue<Vec2> reached_ingresses;
  distance_map.set(Vec2{1, 4}, 1);
  move_by_one(distance_map, occupancy_map, Vec2{1, 4}, reached_ingresses);

  std::vector<Vec2> expected_reached_ingresses = {Vec2{1, 1}, Vec2{2, 1}, Vec2{0, 2}, Vec2{1, 2},
                                                  Vec2{2, 2}, Vec2{3, 2}, Vec2{3, 3}, Vec2{4, 3},
                                                  Vec2{3, 4}, Vec2{4, 4}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(reached_ingresses);
  std::vector<DistanceT> expected_distances = {NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               2,
                                               2,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,

                                               2,
                                               2,
                                               2,
                                               2,
                                               NOT_REACHABLE,

                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               NOT_REACHABLE,
                                               2,
                                               2,

                                               NOT_REACHABLE,
                                               1,
                                               NOT_REACHABLE,
                                               2,
                                               2};
  EXPECT_EQ(distance_map.map(), expected_distances);
  EXPECT_TRUE(std::is_permutation(expected_reached_ingresses.begin(),
                                  expected_reached_ingresses.end(),
                                  actual_reached_ingresses.begin()));
}