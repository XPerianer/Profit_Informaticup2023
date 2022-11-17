#include <gtest/gtest.h>

#include "serialization.hpp"

TEST(Serialization, ReturnsEmptyOutputForEmptyInput) {
  EXPECT_EQ("[]", nlohmann::to_string(serialization::serialize({})));
}
