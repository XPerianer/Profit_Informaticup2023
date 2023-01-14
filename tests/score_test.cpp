
#include "score.hpp"

#include <gtest/gtest.h>

#include "field_state.hpp"
#include "geometry/rotation.hpp"

using namespace profit;

class ScoreTest : public testing::Test {
 protected:
  parsing::Input input_ =
      parsing::Input{
          .dimensions = {100, 100},
          .turns = 50,
          .time = 120,
          .products = {Product{ProductType::TYPE0, {10, 0, 0, 0, 0, 0, 0, 0}, 5},
                       Product{ProductType::TYPE1, {10, 10, 0, 0, 0, 0, 0, 0}, 5},
                       Product{ProductType::TYPE2, {10, 10, 0, 0, 0, 0, 0, 0}, 10}},
          .deposits =
              {
                  Deposit{{0, 0}, {2, 2}, ResourceType::TYPE0},
                  Deposit{{0, 0}, {2, 2}, ResourceType::TYPE1},
                  Deposit{{7, 0}, {3, 3}, ResourceType::TYPE0},
                  Deposit{{10, 13}, {8, 3}, ResourceType::TYPE1},
              },
          .obstacles = {},
      };
  FieldState field_state_ = from_input(input_);
};

TEST_F(ScoreTest, EmptyScore) {
  auto calculated_score = score(field_state_, 10, input_);
  EXPECT_EQ(calculated_score, 0);
}

TEST_F(ScoreTest, EvenDepositHasUndeliveredExcess) {
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
    EXPECT_EQ(calculated_score, 5);
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

TEST_F(ScoreTest, TwoDepositsUndeliveredExcess) {
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
    EXPECT_EQ(calculated_score, 5);
  }
}

TEST_F(ScoreTest, TwoDepositsMultiplePipelinesDeliverAll) {
  auto factory = Factory{
      .handle = {10, 0},
      .type = FactoryType::TYPE2,
  };
  field_state_.factories.insert({0, factory});
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 2,
      .parts =
          {
              Mine{.handle = {3, 5}, .rotation = profit::Rotation::LEFT_TO_RIGHT},
              Conveyor3{.handle = {7, 6}, .rotation = profit::Rotation::LEFT_TO_RIGHT},
              Conveyor3{.handle = {9, 5}, .rotation = profit::Rotation::LEFT_TO_RIGHT},
          },
  });
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 3,
      .parts =
          {
              Mine{.handle = {12, 10}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {14, 8}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {13, 6}, .rotation = profit::Rotation::DOWN_TO_UP},
          },
  });
  field_state_.add_pipeline(Pipeline{
      .factory_id = 0,
      .deposit_id = 3,
      .parts =
          {
              Mine{.handle = {16, 10}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {16, 8}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {17, 6}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {16, 4}, .rotation = profit::Rotation::DOWN_TO_UP},
              Conveyor3{.handle = {15, 2}, .rotation = profit::Rotation::DOWN_TO_UP},
          },
  });

  std::vector<Turn> empty_at = when_are_deposits_empty(field_state_, input_);

  {
    auto calculated_score = score(field_state_, 5, input_);
    auto ressources_from_a = ressources_at_turn(field_state_.pipelines.at(0), empty_at[2], 5);
    auto ressources_from_b_1 = ressources_at_turn(field_state_.pipelines.at(1), empty_at[3], 5);
    auto ressources_from_b_2 = ressources_at_turn(field_state_.pipelines.at(2), empty_at[3], 5);
    EXPECT_EQ(calculated_score, 0);
    EXPECT_EQ(ressources_from_a, 3);
    EXPECT_EQ(ressources_from_b_1, 3);
    EXPECT_EQ(ressources_from_b_2, 0);
  }

  {
    auto calculated_score = score(field_state_, 10, input_);
    auto ressources_from_a = ressources_at_turn(field_state_.pipelines.at(0), empty_at[2], 10);
    auto ressources_from_b_1 = ressources_at_turn(field_state_.pipelines.at(1), empty_at[3], 10);
    auto ressources_from_b_2 = ressources_at_turn(field_state_.pipelines.at(2), empty_at[3], 10);
    EXPECT_EQ(ressources_from_a, 18);
    EXPECT_EQ(ressources_from_b_1, 18);
    EXPECT_EQ(ressources_from_b_2, 12);
    EXPECT_EQ(calculated_score, 10);
  }
  {
    auto calculated_score = score(field_state_, 19, input_);
    auto ressources_from_a = ressources_at_turn(field_state_.pipelines.at(0), empty_at[2], 19);
    auto ressources_from_b_1 = ressources_at_turn(field_state_.pipelines.at(1), empty_at[3], 19);
    auto ressources_from_b_2 = ressources_at_turn(field_state_.pipelines.at(2), empty_at[3], 19);
    EXPECT_EQ(ressources_from_a, 45);
    EXPECT_EQ(ressources_from_b_1, 45);
    EXPECT_EQ(ressources_from_b_2, 39);
    EXPECT_EQ(calculated_score, 40);
  }

  {
    auto calculated_score = score(field_state_, 30, input_);
    auto ressources_from_a = ressources_at_turn(field_state_.pipelines.at(0), empty_at[2], 30);
    auto ressources_from_b_1 = ressources_at_turn(field_state_.pipelines.at(1), empty_at[3], 30);
    auto ressources_from_b_2 = ressources_at_turn(field_state_.pipelines.at(2), empty_at[3], 30);
    EXPECT_EQ(ressources_from_a, 45);
    EXPECT_EQ(ressources_from_b_1, 60);
    EXPECT_EQ(ressources_from_b_2, 60);
    EXPECT_EQ(calculated_score, 40);
  }
}