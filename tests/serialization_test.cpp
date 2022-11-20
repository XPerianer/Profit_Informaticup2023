#include "serialization.hpp"

#include <gtest/gtest.h>

#include "utils.hpp"

using namespace serialization;

TEST(Serialization, EmptyInput) { EXPECT_EQ(nlohmann::json::array(), serialize({})); }

TEST(Serialization, ReturnObjectsAsArray) {
  auto combiner = Combiner{{1, 2}, UP_TO_DOWN};
  std::vector<PlaceableObject> objects{combiner, combiner};
  auto serialized_combiner = serialize_object(combiner);
  nlohmann::json expected_json = {serialized_combiner, serialized_combiner};
  EXPECT_EQ(expected_json, serialize(objects));
}

TEST(Serialization, SerializeCombiner) {
  EXPECT_EQ(nlohmann::json({{"type", "combiner"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Combiner{{15, 10}, UP_TO_DOWN}));
}

TEST(Serialization, SerializeConveyor) {
  EXPECT_EQ(nlohmann::json({{"type", "conveyor"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Conveyor3{{15, 10}, UP_TO_DOWN}));
  EXPECT_EQ(nlohmann::json({{"type", "conveyor"}, {"subtype", 5}, {"x", 15}, {"y", 10}}),
            serialize_object(Conveyor4{{15, 10}, UP_TO_DOWN}));
}

TEST(Serialization, SerializeFactory) {
  EXPECT_EQ(nlohmann::json({{"type", "factory"}, {"subtype", 8}, {"x", 15}, {"y", 10}}),
            serialize_object(Factory{{15, 10}, 8}));
}

TEST(Serialization, SerializeMine) {
  EXPECT_EQ(nlohmann::json({{"type", "mine"}, {"subtype", 1}, {"x", 15}, {"y", 10}}),
            serialize_object(Mine{{15, 10}, UP_TO_DOWN}));
}