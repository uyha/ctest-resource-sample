#include "cmake-resource-parsing.hpp"

#include <iostream>

int main() {
  auto types = ctest::types_of(1).value();
  for (auto const &type : types) {
    std::cout << type << '\n';
  }
  {
    auto const resources = ctest::resources_of(0, "resource").value();
    auto index           = 0;

    for (auto const &[id, slots] : resources) {
      std::cout << "id: " << id << ", slots: " << slots << '\n';
    }
  }
}
