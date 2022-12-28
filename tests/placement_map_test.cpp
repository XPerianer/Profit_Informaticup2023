#include "fields/placement_map.hpp"

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

using namespace geometry;
using namespace profit;
using namespace parsing;

// NOLINTBEGIN(readability-identifier-length): Keep visualization of expected fields comprehensible
const Placeability i = INVALID;
const Placeability v = VALID;
// NOLINTEND(readability-identifier-length)

class PlacementMapTest : public testing::Test {
 public:
  static void with_example(const std::string_view& example,
                           const std::vector<Placeability>& expected) {
    std::istringstream stream{std::string{example}};
    auto input = parsing::parse(stream);
    OccupancyMap occupancy_map = occupancies_from(input);
    std::vector<Deposit> deposits = get_deposits(input);
    ConnectedComponentsWrapper components_wrapper(static_cast<DepositId>(deposits.size()),
                                                  occupancy_map.dimensions());
    std::vector<DistanceMap> distance_maps;
    distance_maps.reserve(deposits.size());

    for (size_t i = 0; i < deposits.size(); i++) {
      distance_maps.emplace_back(distances_from(deposits[i], occupancy_map, &components_wrapper,
                                                static_cast<DepositId>(i)));
    }

    DistanceMap merged = merge(distance_maps);
    PlacementMap placement_map = placements_for<Factory::DIMENSIONS>(occupancy_map, merged);

    EXPECT_THAT(placement_map.map(), testing::ElementsAreArray(expected));
  }
};

TEST(PlacementMap, Task1) {
  PlacementMapTest::with_example(
      examples::TASK1,
      {
          // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, Task2) { PlacementMapTest::with_example(examples::TASK2, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, Task3) { PlacementMapTest::with_example(examples::TASK3, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, 
      v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, v, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, Task4) { PlacementMapTest::with_example(examples::TASK4, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, TrickyConveyorConnection) { PlacementMapTest::with_example(examples::TRICKY_CONVEYOR_CONNECTION, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, VeryStrongContained) { PlacementMapTest::with_example(examples::VERY_STRONG_CONSTRAINED, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, v, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, VerticallySplitMap) { PlacementMapTest::with_example(examples::VERTICALLY_SPLIT_MAP, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}

TEST(PlacementMap, MultipleConnectedComponents) { PlacementMapTest::with_example(examples::MULTIPLE_CONNECTED_COMPONENTS, {
    // clang-format off
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, 
      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i,
    // clang-format on 
    });
}
