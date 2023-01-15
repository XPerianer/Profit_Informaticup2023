#include "mdkp.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "io/parsing.hpp"
#include "miscellany/utils.hpp"
#include "objects/product.hpp"
#include "objects/resource_type.hpp"

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

TEST_F(PechTest, Task1) {
  AvailableResources resources = {125, 125, 245};
  Product product = {
      .type = profit::Subtype::TYPE0, .requirements{3, 3, 3, 0, 0, 0, 0, 0}, .points = 10};
  auto output = pech(resources, {product});
  std::vector<ProductCount> expected = {41};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, Task2) {
  AvailableResources resources = {125};
  Product product = {
      .type = profit::Subtype::TYPE0, .requirements{10, 0, 0, 0, 0, 0, 0, 0}, .points = 10};
  auto output = pech(resources, {product});
  std::vector<ProductCount> expected = {12};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, Task3) {
  AvailableResources resources = {245, 45};
  Product product = {
      .type = profit::Subtype::TYPE0, .requirements{36, 3, 0, 0, 0, 0, 0, 0}, .points = 10};
  auto output = pech(resources, {product});
  std::vector<ProductCount> expected = {6};
  EXPECT_EQ(expected, output);
}

TEST_F(PechTest, Task4) {
  AvailableResources resources = {360, 360, 360, 360};
  Product product_one = {
      .type = profit::Subtype::TYPE0, .requirements{10, 10, 0, 0, 0, 0, 0, 0}, .points = 10};
  Product product_two = {
      .type = profit::Subtype::TYPE0, .requirements{0, 0, 10, 10, 0, 0, 0, 0}, .points = 10};
  auto output = pech(resources, {product_one, product_two});
  std::vector<ProductCount> expected = {36, 36};
  EXPECT_EQ(expected, output);
}