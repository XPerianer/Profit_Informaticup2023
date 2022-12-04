#include <gtest/gtest.h>

#include <algorithm>
#include <vector>

#include "geometry/rectangle.hpp"
#include "geometry/vec2.hpp"

using namespace geometry;

TEST(Rectangle, FromDimensions) {
  auto actual = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{5, 5});
  auto expected_bottom_right = Vec2{6, 6};
  auto expected_max_coord = Vec2{5, 5};
  EXPECT_EQ(actual.bottom_right(), expected_bottom_right);
  EXPECT_EQ(actual.max_coord(), expected_max_coord);
}

TEST(Rectangle, FromBottomRight) {
  auto actual = Rectangle::from_top_left_and_bottom_right(Vec2{1, 1}, Vec2{6, 6});
  auto expected_dimensions = Vec2{5, 5};
  auto expected_max_coord = Vec2{5, 5};
  EXPECT_EQ(actual.dimensions(), expected_dimensions);
  EXPECT_EQ(actual.max_coord(), expected_max_coord);
}

TEST(Rectangle, IterationRange) {
  auto rect = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{5, 5});
  std::vector<Vec2> expected = {Vec2{1, 1}, Vec2{1, 2}, Vec2{1, 3}, Vec2{1, 4}, Vec2{1, 5},
                                Vec2{2, 1}, Vec2{2, 2}, Vec2{2, 3}, Vec2{2, 4}, Vec2{2, 5},
                                Vec2{3, 1}, Vec2{3, 2}, Vec2{3, 3}, Vec2{3, 4}, Vec2{3, 5},
                                Vec2{4, 1}, Vec2{4, 2}, Vec2{4, 3}, Vec2{4, 4}, Vec2{4, 5},
                                Vec2{5, 1}, Vec2{5, 2}, Vec2{5, 3}, Vec2{5, 4}, Vec2{5, 5}};
  std::vector<Vec2> actual = {};
  for (Vec2 coordinate : rect) {
    actual.emplace_back(coordinate);
  }

  EXPECT_TRUE(std::is_permutation(actual.begin(), actual.end(), expected.begin()));
}

TEST(Rectangle, OuterCells) {
  auto rect = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{5, 5});
  auto actual = outer_connected_border_cells(rect);
  std::vector<Vec2> expected = {Vec2{1, 0}, Vec2{2, 0}, Vec2{3, 0}, Vec2{4, 0}, Vec2{5, 0},
                                Vec2{0, 1}, Vec2{6, 1}, Vec2{0, 2}, Vec2{6, 2}, Vec2{0, 3},
                                Vec2{6, 3}, Vec2{0, 4}, Vec2{6, 4}, Vec2{0, 5}, Vec2{6, 5},
                                Vec2{1, 6}, Vec2{2, 6}, Vec2{3, 6}, Vec2{4, 6}, Vec2{5, 6}};
  EXPECT_TRUE(std::is_permutation(actual.begin(), actual.end(), expected.begin()));
}

TEST(Rectangle, 1x1OuterCells) {
  auto rect = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{1, 1});
  auto actual = outer_connected_border_cells(rect);
  auto expected = {Vec2{1, 0}, Vec2{0, 1}, Vec2{2, 1}, Vec2{1, 2}};
  EXPECT_TRUE(std::is_permutation(actual.begin(), actual.end(), expected.begin()));
}

TEST(Rectangle, IsOnBorder) {
  auto rect = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{5, 5});
  auto border_cells = {Vec2{1, 1}, Vec2{2, 1}, Vec2{3, 1}, Vec2{4, 1}, Vec2{5, 1}, Vec2{1, 1},
                       Vec2{5, 1}, Vec2{1, 2}, Vec2{5, 2}, Vec2{1, 3}, Vec2{5, 3}, Vec2{1, 4},
                       Vec2{5, 4}, Vec2{1, 5}, Vec2{2, 5}, Vec2{3, 5}, Vec2{4, 5}, Vec2{5, 5}};
  EXPECT_TRUE(
      std::ranges::all_of(border_cells, [&](auto cell) { return is_on_border(rect, cell); }));
}

TEST(Rectangle, ZeroSized) {
  auto rect = Rectangle::from_top_left_and_dimensions(Vec2{1, 1}, Vec2{0, 0});
  auto expected_min = Vec2{1, 1};
  EXPECT_EQ(rect.min_coord(), expected_min);
  EXPECT_EQ(rect.max_coord(), expected_min);
}