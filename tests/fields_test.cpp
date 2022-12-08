#include <gmock/gmock.h>
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

std::vector<Vec2> queue_to_vector(std::queue<Vec2>&& queue) {
  std::vector<Vec2> result;
  while (!queue.empty()) {
    result.push_back(queue.front());
    queue.pop();
  }
  return result;
}

// NOLINTBEGIN(readability-identifier-length): Keep Visualization of expected fields comprehensible
const CellOccupancy o = CellOccupancy::EMPTY;
const CellOccupancy x = CellOccupancy::BLOCKED;
const CellOccupancy e = CellOccupancy::EGRESS;
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
      // clang-format off
      o, o, o, o, o,
      o, o, o, o, o,
      o, o, x, x, o,
      o, o, x, x, o,
      o, o, o, o, o,
      // clang-format on
  };
  EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
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
      // clang-format off
      o, o, o, o, o,
      o, o, o, o, o,
      o, o, e, e, e,
      o, o, e, x, e,
      o, o, e, e, e,
      // clang-format on
  };
  EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
}

TEST(OccupancyMap, Task2) {
  std::istringstream stream{std::string{examples::TASK2}};
  auto input = parsing::parse(stream);
  OccupancyMap occupancy_map = occupancies_from(input);
  std::vector<CellOccupancy> expected = {
      // clang-format off
      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, x, x, x, e, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, x, o, o, o, o, o,
      e, x, x, x, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      e, e, e, e, e, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o, o,
      // clang-format on
  };
  EXPECT_THAT(occupancy_map.map(), testing::ElementsAreArray(expected));
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

TEST(DistanceMap, FixedMinePlacement) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, e);
  std::queue<Vec2> reached_ingresses;
  update_reachability_if_placeable(distance_map, occupancy_map,
                                   Mine::with_ingress(Vec2{0, 1}, Rotation::LEFT_TO_RIGHT), 1,
                                   reached_ingresses);
  std::vector<Vec2> expected_reached_ingresses = {Vec2{3, 0}, Vec2{4, 1}, Vec2{3, 2}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(std::move(reached_ingresses));
  // clang-format off
  std::vector<DistanceT> expected_distances = {n, n, n, 1, n,
                                               n, n, n, n, 1,
                                               n, n, n, 1, n,
                                               n, n, n, n, n,
                                               n, n, n, n, n};
  // clang-format on
  EXPECT_THAT(distance_map.map(), testing::ElementsAreArray(expected_distances));
  EXPECT_TRUE(std::ranges::is_permutation(expected_reached_ingresses, actual_reached_ingresses));
}

TEST(DistanceMap, MinePlacements) {
  DistanceMap distance_map(Vec2{5, 5});
  OccupancyMap occupancy_map(Vec2{5, 5});
  occupancy_map.set(Vec2{0, 0}, CellOccupancy::EGRESS);
  std::queue<Vec2> reached_ingresses;
  for (Vec2 possible_ingress_location : {Vec2{0, 1}, Vec2{1, 0}}) {
    for (auto rotation : ROTATIONS) {
      update_reachability_if_placeable(distance_map, occupancy_map,
                                       Mine::with_ingress(possible_ingress_location, rotation), 1,
                                       reached_ingresses);
    }
  }
  std::vector<Vec2> expected_reached_ingresses = {Vec2{3, 0}, Vec2{4, 1}, Vec2{3, 2},
                                                  Vec2{0, 3}, Vec2{2, 3}, Vec2{1, 4}};
  std::vector<Vec2> actual_reached_ingresses = queue_to_vector(std::move(reached_ingresses));
  // clang-format off
  std::vector<DistanceT> expected_distances = {n, n, n, 1, n,
                                               n, n, n, n, 1,
                                               n, n, n, 1, n,
                                               1, n, 1, n, n,
                                               n, 1, n, n, n};
  // clang-format on
  EXPECT_THAT(distance_map.map(), testing::ElementsAreArray(expected_distances));
  EXPECT_TRUE(std::ranges::is_permutation(expected_reached_ingresses, actual_reached_ingresses));
}

TEST(DistanceMap, Initialization) {
  OccupancyMap occupancy_map(Vec2{5, 5});
  Deposit deposit{.handle = Vec2{0, 0}, .dimensions = Vec2{1, 1}};
  occupancy_map.set(Vec2{0, 0}, CellOccupancy::EGRESS);
  DistanceMap distance_map = distances_from(deposit, occupancy_map);

  // clang-format off
  std::vector<DistanceT> expected_distances = {n, n, 2, 1, 2,
                                               n, 2, 2, 2, 1,
                                               2, 2, 2, 1, 2,
                                               1, 2, 1, 2, 2,
                                               2, 1, 2, 2, 2};
  // clang-format on
  EXPECT_EQ(distance_map.map().size(), 25);
  EXPECT_THAT(distance_map.map(), testing::ElementsAreArray(expected_distances));
}
