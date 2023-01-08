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
#include "utils.hpp"

using namespace geometry;
using namespace profit;
using namespace parsing;

class ConnectionTest : public testing::Test {
 public:
  static Input from_string(const std::string_view& example) {
    // Fill stream with file content
    std::stringstream stream{std::string{example}};
    return parsing::parse(stream);
  }
};

TEST(Connect, CantConnectImpossibleThings) {
  auto input = ConnectionTest::from_string(examples::VERTICALLY_SPLIT_MAP);
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

  FactoryId factory_id = 0;

  FieldState state = from_input(input);
  state.factories[factory_id] = unreachable_factory;

  auto pipeline_id = connect(deposit, factory_id, &state);
  EXPECT_EQ(pipeline_id, INVALID_PIPELINE_ID);
}

TEST(Connect, ConnectWithMineand3Conveyor) {
  auto input = ConnectionTest::from_string(examples::CONSTRAINED_TO_MINE_AND_CONVEYOR3);
  auto deposit = input.deposits.at(0);

  auto factory = Factory{
      .handle = {0, 12},
      .type = FactoryType::TYPE0,
  };

  FieldState state = from_input(input);

  FactoryId factory_id = 0;
  state.factories[factory_id] = factory;

  auto pipeline_id = connect(deposit, factory_id, &state);

  Pipeline pipeline = state.pipelines[pipeline_id];

  EXPECT_EQ(state.factories[pipeline.factory_id], factory);
  EXPECT_EQ(pipeline.parts.size(), 2);
  for (auto part : pipeline.parts) {
    std::visit(utils::Overloaded{
                   [](Mine mine) {
                     EXPECT_EQ(mine.handle.x(), 0);
                     EXPECT_EQ(mine.handle.y(), 6);
                   },
                   [](Conveyor3 conveyor) {
                     EXPECT_EQ(conveyor.handle.x(), 0);
                     EXPECT_EQ(conveyor.handle.y(), 10);
                   },
                   [](Conveyor4 /**/) { FAIL("Should not be reached"); },
                   [](Factory /**/) { FAIL("Should not be reached"); },
                   [](Combiner /**/) { FAIL("Should not be reached"); },
               },
               part);
  }
}
