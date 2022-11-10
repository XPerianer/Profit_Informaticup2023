#include <iostream>
#include <vector>

#include "Field.hpp"
#include "InputParser.hpp"

int main() {
  Input input = InputParser::parse(std::cin);
  Field field({input.width, input.height});
}
