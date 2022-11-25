#pragma once

#include <initializer_list>

enum class Rotation {
  LEFT_TO_RIGHT = 0,
  UP_TO_DOWN = 1,
  RIGHT_TO_LEFT = 2,
  DOWN_TO_UP = 3,
};

constexpr auto rotations = {Rotation::DOWN_TO_UP, Rotation::LEFT_TO_RIGHT, Rotation::UP_TO_DOWN,
                            Rotation::RIGHT_TO_LEFT};