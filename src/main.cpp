#include <iostream>
#include <vector>

#include "parsing.hpp"

int main() {
  parsing::Input input = parsing::parse(std::cin);
  (void)input;
}
