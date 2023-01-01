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
  Product goodProduct, badProduct;

  void SetUp() override {
    goodProduct.points = 10;
    goodProduct.requirements[ResourceType::TYPE0] = 2;

    // Product_bad is bad because it has lower points than product_good
    badProduct.points = 5;
    badProduct.requirements[ResourceType::TYPE0] = 2;
  }
};

TEST_F(PechTest, OutputsCorrectNumberOfElements) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  auto output = pech(resources, {goodProduct});

  std::vector<ProductCount> expected = {4};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, ChoosesProductWithHigherScore) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  auto output = pech(resources, {badProduct, goodProduct});
  std::vector<ProductCount> expected = {0, 4};
  EXPECT_EQ(expected, output);

  // Swap parameters to know that we did not get lucky
  output = pech(resources, {goodProduct, badProduct});
  expected = {4, 0};
  EXPECT_EQ(expected, output);
}
