#include "mdkp.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "io/parsing.hpp"
#include "product.hpp"
#include "resource_type.hpp"
#include "utils.hpp"

using namespace profit;

TEST(Pech, OutputsCorrectNumberOfElements) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  Product product_one;
  product_one.points = 10;
  product_one.requirements[ResourceType::TYPE0] = 2;
  auto output = pech(resources, {product_one});

  std::vector<ProductCount> expected = {4};
  EXPECT_EQ(expected, output);
}

TEST(Pech, ChoosesProductWithHigherScore) {
  AvailableResources resources = {8, 8, 8, 8, 8, 8, 8, 8};
  Product product_good;
  product_good.points = 10;
  product_good.requirements[ResourceType::TYPE0] = 2;

  // Product is bad because it has lower points than product_good
  Product product_bad;
  product_bad.points = 5;
  product_bad.requirements[ResourceType::TYPE0] = 2;
  auto output = pech(resources, {product_bad, product_good});
  std::vector<ProductCount> expected = {0, 4};
  EXPECT_EQ(expected, output);

  // Swap parameters to know that we did not get lucky
  output = pech(resources, {product_good, product_bad});
  expected = {4, 0};
  EXPECT_EQ(expected, output);
}
