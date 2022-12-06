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

std::vector<Vec2> queue_to_vector(std::queue<Vec2>& queue) {
  std::vector<Vec2> result;
  while (!queue.empty()) {
    result.push_back(queue.front());
    queue.pop();
  }
  return result;
}

// NOLINTBEGIN(readability-identifier-length): Keep Visualization of expected fields comprehensible
const int o = static_cast<int>(CellOccupancy::EMPTY);
const int x = static_cast<int>(CellOccupancy::BLOCKED);
const int c = static_cast<int>(CellOccupancy::CONVEYOR_CROSSING);
const int i = static_cast<int>(CellOccupancy::INGRESS);
const int e = static_cast<int>(CellOccupancy::EGRESS);
const int n = static_cast<int>(NOT_REACHABLE);
// NOLINTEND(readability-identifier-length)

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
      o, o, o, o, o,

      o, o, o, o, o,

      o, o, x, x, o,

      o, o, x, x, o,

      o, o, o, o, o,
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
  OccupancyMap occupancy_map = occupancies_from(input);
  std::vector<CellOccupancy> expected = {
      o, o, o, o, o,

      o, o, o, o, o,

      o, o, e, e, e,

      o, o, e, x, e,

      o, o, e, e, e,
  };
  EXPECT_EQ(occupancy_map.map(), expected);
}

TEST(OccupancyMap, Task2) {
  std::istringstream stream{std::string{examples::TASK2}};
  auto input = parsing::parse(stream);
  OccupancyMap occupancy_map = occupancies_from(input);
  std::vector<CellOccupancy> expected = {
      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,

      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,

      e, x, x, x, e, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, o, o, o, o, o,

      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,

      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,

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

TEST(DistanceMap, FixedMinePlacement) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, e);
  std::queue<Vec2> reached_ingresses;
  place_objects(distance_map, occupancy_map,
                {Mine::with_ingress(Vec2{0, 1}, Rotation::LEFT_TO_RIGHT)}, 1, reached_ingresses);
  std::vector<Vec2> expected_reached_ingresses = {Vec2{3, 0}, Vec2{4, 1}, Vec2{3, 2}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(reached_ingresses);
  std::vector<DistanceT> expected_distances = {n, n, n, 1, n,

                                               n, n, n, n, 1,

                                               n, n, n, 1, n,

                                               n, n, n, n, n,

                                               n, n, n, n, n};
  EXPECT_EQ(distance_map.map(), expected_distances);
  EXPECT_TRUE(std::ranges::is_permutation(expected_reached_ingresses, actual_reached_ingresses));
}

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
  std::vector<DistanceT> expected_distances = {n, n, n, 1, n,

                                               n, n, n, n, 1,

                                               n, n, n, 1, n,

                                               1, n, 1, n, n,

                                               n, 1, n, n, n};
  EXPECT_EQ(distance_map.map(), expected_distances);
  EXPECT_TRUE(std::ranges::is_permutation(expected_reached_ingresses, actual_reached_ingresses));
}

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
  std::vector<DistanceT> expected_distances = {n, n, n, n, n,

                                               n, 2, 2, n, n,

                                               2, 2, 2, 2, n,

                                               n, n, n, 2, 2,

                                               n, 1, n, 2, 2};
  EXPECT_THAT(distance_map.map(), ElementsAre(expected_distances));
  EXPECT_TRUE(std::ranges::is_permutation(expected_reached_ingresses, actual_reached_ingresses));
}
