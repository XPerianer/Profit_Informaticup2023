
#include "score.hpp"

#include <gtest/gtest.h>

#include "field_state.hpp"

using namespace profit;

class ScoreTest : public testing::Test {
 protected:
  parsing::Input input_ = parsing::Input{
      .dimensions = {100, 100},
      .turns = 50,
      .time = 120,
      .products = {Product{ProductType::TYPE0, {10, 0, 0, 0, 0, 0, 0, 0}, 5},
                   Product{ProductType::TYPE1, {10, 10, 0, 0, 0, 0, 0, 0}, 5}},
      .deposits =
          {
              Deposit{{0, 0}, {2, 2}, ResourceType::TYPE0},
              Deposit{{0, 0}, {2, 2}, ResourceType::TYPE1},
          },
      .obstacles = {},
  };
  FieldState field_state_ = from_input(input_);
};

TEST_F(ScoreTest, EmptyScore) {
  auto calculated_score = score(field_state_, 10, input_);
  EXPECT_EQ(calculated_score, 0);
}

TEST_F(ScoreTest, SimpleScore) {
  auto factory = Factory{
      .handle = {25, 10},
      .type = FactoryType::TYPE0,
  };
  field_state_.factories.insert({0, factory});
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 0,
      .parts =
          {
              Mine{},
          },
  });
  {
    auto calculated_score = score(field_state_, 0, input_);
    EXPECT_EQ(calculated_score, 0);
  }

  {
    auto calculated_score = score(field_state_, 5, input_);
    EXPECT_EQ(calculated_score, 0);
  }

  {
    auto calculated_score = score(field_state_, 6, input_);
    EXPECT_EQ(calculated_score, 5);
  }

  {
    auto calculated_score = score(field_state_, 9, input_);
    EXPECT_EQ(calculated_score, 10);
  }

  {
    auto calculated_score = score(field_state_, 10, input_);
    // Inital Resources in Deposit / 10 * Score
    int inital_resources_in_deposit = 20;
    int product_score = 5;
    int required_resources = 10;
    EXPECT_EQ(calculated_score, inital_resources_in_deposit * product_score / required_resources);
  }
}

TEST_F(ScoreTest, NoScoreIfNotAllResourcesAvailable) {
  auto factory = Factory{
      .handle = {25, 10},
      .type = FactoryType::TYPE1,
  };
  field_state_.factories.insert({0, factory});
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 0,
      .parts =
          {
              Mine{},
          },
  });
  auto calculated_score = score(field_state_, 10, input_);
  EXPECT_EQ(calculated_score, 0);
}

TEST_F(ScoreTest, TwoDeposits) {
  auto factory = Factory{
      .handle = {25, 10},
      .type = FactoryType::TYPE1,
  };
  field_state_.factories.insert({0, factory});
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 0,
      .parts =
          {
              Mine{.handle = {10, 10}},
          },
  });
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 1,
      .parts =
          {
              Mine{.handle = {10, 10}},
              Conveyor3{.handle = {10, 10}},
          },
  });
  {
    auto calculated_score = score(field_state_, 9, input_);
    EXPECT_EQ(calculated_score, 5);
  }

  {
    auto calculated_score = score(field_state_, 10, input_);
    EXPECT_EQ(calculated_score, 10);
  }
}
