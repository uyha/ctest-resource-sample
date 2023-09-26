#include "cmake-resource-parsing.hpp"

#include <iostream>

int main() {
  auto const groups = river::get_resource_groups().value();

  for (auto const &group : groups) {
    for (auto const &[type, resources] : group) {
      for (auto const &[id, slots] : resources) {
        std::cout << "id: " << id << ", slots: " << slots << '\n';
      }
    }
  }
}
