// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#pragma once

#include <charconv>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ctest {
struct Resource {
  std::string id;
  std::size_t slots;
};

/* NOLINTBEGIN(*swappable-parameters,*mt-unsafe,*pointer-arithmetic) */
inline auto parse_resource_types(std::string_view source) -> std::vector<std::string> {
  auto buffer = std::string{};
  auto result = std::vector<std::string>{};

  for (auto ch : source) {
    if (ch == ',') {
      result.push_back(std::move(buffer));
    } else {
      buffer += ch;
    }
  }
  if (not buffer.empty()) {
    result.push_back(std::move(buffer));
  }

  return result;
}

inline auto set_resource(std::string_view type, std::string_view value, Resource &res) -> bool {
  if (type == "id") {
    res.id = value;
    return true;
  }

  if (type == "slots") {
    std::size_t slots = 0;
    auto conv_result  = std::from_chars(value.data(), value.data() + value.size(), slots);

    if (conv_result.ec != std::errc{}) {
      return false;
    }

    res.slots = slots;

    return true;
  }

  return false;
}

inline auto parse_resource(std::string_view source) -> std::optional<std::vector<Resource>> {
  auto type_buffer     = std::string{};
  auto value_buffer    = std::string{};
  auto *current_buffer = &type_buffer;

  auto result = std::vector<Resource>{};

  auto resource = Resource{};

  for (auto ch : source) {
    if (ch == ':') {
      current_buffer = &value_buffer;
    } else if (ch == ',') {
      if (not set_resource(type_buffer, value_buffer, resource)) {
        return std::nullopt;
      }

      type_buffer.clear();
      value_buffer.clear();
      current_buffer = &type_buffer;
    } else if (ch == ';') {
      if (not set_resource(type_buffer, value_buffer, resource)) {
        return std::nullopt;
      }

      type_buffer.clear();
      value_buffer.clear();
      current_buffer = &type_buffer;

      result.push_back(resource);
      resource = Resource{};
    } else {
      (*current_buffer) += ch;
    }
  }

  if (not set_resource(type_buffer, value_buffer, resource)) {
    return std::nullopt;
  }
  result.push_back(resource);

  return result;
}

inline auto count() -> std::optional<std::size_t> {
  /* NOLINTNEXTLINE(*mt-unsafe) */
  char const *group_count_ptr = std::getenv("CTEST_RESOURCE_GROUP_COUNT");
  if (group_count_ptr == nullptr) {
    return std::nullopt;
  }

  std::size_t result = 0;
  {
    auto const *begin = group_count_ptr;
    /* NOLINTNEXTLINE(*pointer-arithmetic) */
    auto const *end  = group_count_ptr + std::strlen(group_count_ptr);
    auto conv_result = std::from_chars(begin, end, result);
    if (conv_result.ec != std::errc{}) {
      return std::nullopt;
    }
  }

  return result;
}

inline auto types_of(std::size_t num) -> std::optional<std::vector<std::string>> {
  char const *group = std::getenv(("CTEST_RESOURCE_GROUP_" + std::to_string(num)).c_str());

  if (group == nullptr) {
    return std::nullopt;
  }
  return parse_resource_types(group);
}

inline auto resources_of(std::size_t num, char const *type) -> std::optional<std::vector<Resource>> {
  auto env_string = std::string("CTEST_RESOURCE_GROUP_") + std::to_string(num) + '_';
  for (auto ptr = type; *ptr != '\0'; ++ptr) {
    env_string += std::toupper(*ptr);
  }
  /* NOLINTNEXTLINE(*mt-unsafe) */
  auto const content = std::getenv((env_string).c_str());
  if (content == nullptr) {
    return std::nullopt;
  }
  return parse_resource(content);
}
/* NOLINTEND(*swappable-parameters,*mt-unsafe,*pointer-arithmetic) */
} // namespace ctest