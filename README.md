# Parsing CTest resource

This repo demonstrate a way to use CTest resource feature (more about it
[here](https://cmake.org/cmake/help/latest/manual/ctest.1.html#resource-specification-file)) and also provides a C++ header
for parsing the environment variables passed by CTest.

## Parsing library

If you just need to parse the environment variables passed by CTest, you can download the
[cmake-resource-parsing.hpp](https://raw.githubusercontent.com/uyha/cmake-resource-sample/main/cmake-resource-parsing.hpp)
file, include in your project, and use the `river::get_resource_groups` to get those variables in a C++ structure.

## How to run

1. Configure and build the project

```sh
cmake -B build && cmake --build build
```

2. Run the test

```sh
ctest --test-dir build --resource-spec-file $(pwd)/resource.json --verbose
```
