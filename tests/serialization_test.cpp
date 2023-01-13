#include "io/serialization.hpp"

#include <gtest/gtest.h>

#include "miscellany/utils.hpp"
#include "resources/example_tasks.hpp"

using namespace profit;
using namespace serialization;

TEST(Serialization, EmptyInput) { EXPECT_EQ(nlohmann::json::array(), serialize({})); }

TEST(Serialization, ReturnObjectsAsArray) {
  auto combiner = Combiner{{1, 2}, Rotation::UP_TO_DOWN};
  std::vector<PlaceableObject> objects{combiner, combiner};
  auto serialized_combiner = serialize_object(combiner);
  nlohmann::json expected_json = {serialized_combiner, serialized_combiner};
  EXPECT_EQ(expected_json, serialize(objects));
}

TEST(Serialization, ReturnsInput) {
  std::istringstream input_stream{std::string{examples::TASK1}};
  nlohmann::json serialized_input = nlohmann::json::parse(input_stream.str());
  auto input = parsing::parse(input_stream);
  auto output = Output{input.dimensions, input.turns,     input.time, input.products,
                       input.deposits,   input.obstacles, {}};
  nlohmann::json serialized_output = serialize_detailed(output);
  EXPECT_EQ(serialized_input, serialized_output);
}

TEST(Serialization, SerializeCombiner) {
  EXPECT_EQ(nlohmann::json({{"type", "combiner"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Combiner{{15, 10}, Rotation::UP_TO_DOWN}));
}

TEST(Serialization, SerializeConveyor) {
  EXPECT_EQ(nlohmann::json({{"type", "conveyor"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Conveyor3{{15, 10}, Rotation::UP_TO_DOWN}));
  EXPECT_EQ(nlohmann::json({{"type", "conveyor"}, {"subtype", 5}, {"x", 15}, {"y", 10}}),
            serialize_object(Conveyor4{{15, 10}, Rotation::UP_TO_DOWN}));
}

TEST(Serialization, SerializeFactory) {
  EXPECT_EQ(nlohmann::json({{"type", "factory"}, {"subtype", 7}, {"x", 15}, {"y", 10}}),
            serialize_object(Factory{{15, 10}, FactoryType::TYPE7}));
}

TEST(Serialization, SerializeMine) {
  EXPECT_EQ(nlohmann::json({{"type", "mine"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Mine{{15, 10}, Rotation::UP_TO_DOWN}));
}

TEST(Serialization, SerializeDeposit) {
  EXPECT_EQ(
      nlohmann::json(
          {{"type", "deposit"}, {"subtype", 1}, {"x", 15}, {"y", 10}, {"width", 5}, {"height", 5}}),
      serialize_object(Deposit{{15, 10}, {5, 5}, ResourceType::TYPE1}));
}

TEST(Serialization, SerializeObstacle) {
  EXPECT_EQ(
      nlohmann::json({{"type", "obstacle"}, {"x", 15}, {"y", 10}, {"width", 5}, {"height", 5}}),
      serialize_object(Obstacle{{15, 10}, {5, 5}}));
}

TEST(Serialization, SerializeProduct) {
  EXPECT_EQ(nlohmann::json({{"type", "product"},
                            {"subtype", 0},
                            {"resources", {10, 0, 0, 0, 0, 0, 0, 0}},
                            {"points", 10}}),
            serialize_product(Product{ProductType::TYPE0, {10, 0, 0, 0, 0, 0, 0, 0}, 10}));
}

TEST(Output, DifferentOrdering) {
  auto output_a =
      Output{.dimensions = {10, 10},
             .turns = 50,
             .time = 120,
             .products = {Product{ProductType::TYPE0, {{10, 10, 0, 0, 0, 0, 0, 0}}, 10},
                          Product{ProductType::TYPE1, {{0, 0, 10, 10, 0, 0, 0, 0}}, 10}},
             .deposits = {Deposit{{0, 0}, {8, 9}, ResourceType::TYPE0}},
             .obstacles =
                 {
                     Obstacle{{8, 0}, {4, 11}},
                     Obstacle{{3, 12}, {15, 3}},
                 },
             .placeable_objects = {Mine{{8, 0}, Rotation::UP_TO_DOWN},
                                   Factory{{3, 12}, FactoryType::TYPE2}}};
  auto output_b = Output{.dimensions = {10, 10},
                         .turns = 50,
                         .time = 120,
                         .products = {Product{ProductType::TYPE1, {0, 0, 10, 10, 0, 0, 0, 0}, 10},
                                      Product{ProductType::TYPE0, {10, 10, 0, 0, 0, 0, 0, 0}, 10}},
                         .deposits = {Deposit{{0, 0}, {8, 9}, ResourceType::TYPE0}},
                         .obstacles =
                             {
                                 Obstacle{{8, 0}, {4, 11}},
                                 Obstacle{{3, 12}, {15, 3}},
                             },
                         .placeable_objects = {Factory{{3, 12}, FactoryType::TYPE2},
                                               Mine{{8, 0}, Rotation::UP_TO_DOWN}}};

  EXPECT_EQ(output_a, output_b);
}
