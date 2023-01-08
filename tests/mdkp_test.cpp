#include "mdkp.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "io/parsing.hpp"
#include "product.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

using namespace profit;

class PechTest : public testing::Test {
 protected:
  Product good_product_, bad_product_;

  void SetUp() override {
    good_product_.points = 10;
    good_product_.requirements[ResourceType::TYPE0] = 2;

    // Product_bad is bad because it has lower points than product_good
    bad_product_.points = 5;
    bad_product_.requirements[ResourceType::TYPE0] = 2;
  }
};

TEST_F(PechTest, OutputsCorrectNumberOfElements) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  auto output = pech(resources, {good_product_});

  std::vector<ProductCount> expected = {4};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, ChoosesProductWithHigherScore) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  auto output = pech(resources, {bad_product_, good_product_});
  std::vector<ProductCount> expected = {0, 4};
  EXPECT_EQ(expected, output);

  // Swap parameters to know that we did not get lucky
  output = pech(resources, {good_product_, bad_product_});
  expected = {4, 0};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, WorksForOneResource) {
  AvailableResources resources = {8};
  auto output = pech(resources, {bad_product_});
  std::vector<ProductCount> expected = {4};
  EXPECT_EQ(expected, output);
}