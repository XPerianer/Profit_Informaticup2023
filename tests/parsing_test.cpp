#include "parsing.hpp"

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "example_tasks.hpp"
#include "utils.hpp"

using namespace parsing;

TEST(Parsing, ParseTaskOne) {
  auto expected = Input{.dimensions = {30, 20},
                        .turns = 50,
                        .time = 300,
                        .products = {Product{ProductType::TYPE0, {3, 3, 3, 0, 0, 0, 0, 0}, 10}},
                        .objects = {
                            Deposit{{1, 1}, {5, 5}, ResourceType::TYPE0},
                            Deposit{{1, 14}, {5, 5}, ResourceType::TYPE1},
                            Deposit{{22, 1}, {7, 7}, ResourceType::TYPE2},
                            Obstacle{{11, 9}, {19, 2}},
                            Obstacle{{11, 1}, {2, 8}},
                        }};
  std::istringstream stream{std::string{examples::TASK1}};
  EXPECT_EQ(expected, parse(stream));
}

TEST(Parsing, ParseTaskTwo) {
  auto expected = Input{.dimensions = {26, 5},
                        .turns = 20,
                        .time = 60,
                        .products = {Product{ProductType::TYPE0, {10, 0, 0, 0, 0, 0, 0, 0}, 10}},
                        .objects = {
                            Obstacle{{5, 2}, {16, 1}},
                            Deposit{{0, 0}, {5, 5}, ResourceType::TYPE0},
                        }};
  std::istringstream stream{std::string{examples::TASK2}};
  EXPECT_EQ(expected, parse(stream));
}

TEST(Parsing, ParseTaskThree) {
  auto expected = Input{.dimensions = {40, 40},
                        .turns = 14,
                        .time = 300,
                        .products{Product{ProductType::TYPE0, {36, 3, 0, 0, 0, 0, 0, 0}, 10}},
                        .objects = {
                            Deposit{{1, 1}, {7, 7}, ResourceType::TYPE0},
                            Deposit{{36, 36}, {3, 3}, ResourceType::TYPE1},
                        }};
  std::istringstream stream{std::string{examples::TASK3}};
  EXPECT_EQ(expected, parse(stream));
}

TEST(Parsing, ParseTaskFour) {
  auto expected =
      Input{.dimensions = {29, 23},
            .turns = 50,
            .time = 120,
            .products = {Product{ProductType::TYPE0, {10, 10, 0, 0, 0, 0, 0, 0}, 10},
                         Product{ProductType::TYPE1, {0, 0, 10, 10, 0, 0, 0, 0}, 10}},
            .objects = {Obstacle{{8, 0}, {4, 11}}, Obstacle{{8, 12}, {4, 11}},
                        Deposit{{0, 0}, {8, 9}, ResourceType::TYPE0},
                        Deposit{{0, 14}, {8, 9}, ResourceType::TYPE1},
                        Deposit{{21, 0}, {8, 9}, ResourceType::TYPE2},
                        Deposit{{21, 14}, {8, 9}, ResourceType::TYPE3}, Obstacle{{17, 0}, {4, 11}},
                        Obstacle{{17, 12}, {4, 11}}, Obstacle{{14, 10}, {1, 3}},
                        Obstacle{{12, 0}, {5, 1}}, Obstacle{{12, 22}, {5, 1}}}};
  std::istringstream stream{std::string{examples::TASK4}};
  EXPECT_EQ(expected, parse(stream));
}

TEST(Input, DifferentOrdering) {
  auto input_a = Input{.dimensions = {10, 10},
                       .turns = 50,
                       .time = 120,
                       .products = {Product{ProductType::TYPE0, {10, 10, 0, 0, 0, 0, 0, 0}, 10},
                                    Product{ProductType::TYPE1, {0, 0, 10, 10, 0, 0, 0, 0}, 10}},
                       .objects = {Obstacle{{8, 0}, {4, 11}}, Obstacle{{3, 12}, {15, 3}},
                                   Deposit{{0, 0}, {8, 9}, ResourceType::TYPE0}}};
  auto input_b = Input{.dimensions = {10, 10},
                       .turns = 50,
                       .time = 120,
                       .products = {Product{ProductType::TYPE1, {0, 0, 10, 10, 0, 0, 0, 0}, 10},
                                    Product{ProductType::TYPE0, {10, 10, 0, 0, 0, 0, 0, 0}, 10}},
                       .objects = {Obstacle{{3, 12}, {15, 3}}, Obstacle{{8, 0}, {4, 11}},
                                   Deposit{{0, 0}, {8, 9}, ResourceType::TYPE0}}};
  EXPECT_EQ(input_a, input_b);
}