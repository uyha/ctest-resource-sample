cmake_minimum_required(VERSION 3.21)
project(CMakeResourceSample VERSION 0.1.0 LANGUAGES CXX)

enable_testing()

add_executable(main main.cpp)
target_compile_features(main PRIVATE cxx_std_17)

add_test(
  NAME main
  COMMAND main
)
set_property(
  TEST main
  PROPERTY
    RESOURCE_GROUPS "2,resource:1;resource:2,resource:20"
)
