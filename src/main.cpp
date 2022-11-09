#include <iostream>
#include <vector>

#include "Field.cpp"
#include "InputParser.cpp"

int main() {
  Input input = InputParser::parse(std::cin);
  Field<int32_t> field(input.width, input.height, input.objects);

  std::cout << field << std::endl;
}
