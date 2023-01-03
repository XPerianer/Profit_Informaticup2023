#include "connect.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
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

class ConnectionTest : public testing::Test {
 public:
  static Input from_file(const std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    EXPECT_TRUE(file.is_open());

    // Fill stream with file content
    std::stringstream stream;
    stream << file.rdbuf();
    return parsing::parse(stream);
  }
};

TEST(Connect, CantConnectImpossibleThings) {
  auto input = ConnectionTest::from_file("../tasks/vertically_split_map.json");

  auto deposit = input.deposits.at(0);

  // Not reachable because of other side of split map
  // In real code this should not happen, as both
  // objects are in different connected components. However,
  // objects could still become unreachable due to placed e.g.
  // placed factories.
  auto unreachable_factory = Factory{
      .handle = {25, 10},
      .type = FactoryType::TYPE0,
  };

  FieldState state = from_input(input);

  auto pipeline_id = connect(deposit, unreachable_factory, &state);
  EXPECT_EQ(pipeline_id, INVALID_PIPELINE_ID);
}
