#pragma once

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace river {
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

inline auto get_group_count() -> std::optional<std::size_t> {
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

inline auto get_groups() -> std::optional<std::vector<std::vector<std::string>>> {
  auto const group_count = get_group_count();
  if (not group_count.has_value()) {
    return std::nullopt;
  }

  auto result = std::vector<std::vector<std::string>>{};

  for (std::size_t i = 0; i < *group_count; ++i) {
    char const *group = std::getenv(("CTEST_RESOURCE_GROUP_" + std::to_string(i)).c_str());

    if (group == nullptr) {
      return std::nullopt;
    }
    result.push_back(parse_resource_types(group));
  }

  return result;
}

inline auto get_resource_groups() -> std::optional<std::vector<std::map<std::string, std::vector<Resource>>>> {
  auto const groups = get_groups();
  if (not groups.has_value()) {
    return std::nullopt;
  }

  auto result = std::vector<std::map<std::string, std::vector<Resource>>>{};

  for (std::size_t i = 0; i < groups->size(); ++i) {
    auto current_group = std::map<std::string, std::vector<Resource>>{};

    for (auto const &type : (*groups)[i]) {
      auto uppercase_type = std::string{};
      std::transform(type.cbegin(), type.cend(), std::back_inserter(uppercase_type), [](auto c) {
        return std::toupper(c);
      });

      char const *resource = std::getenv(("CTEST_RESOURCE_GROUP_" + std::to_string(i) + "_" + uppercase_type).c_str());

      if (resource == nullptr) {
        return std::nullopt;
      }

      if (auto parsed_resource = parse_resource(resource); parsed_resource.has_value()) {
        current_group[type] = std::move(*parsed_resource);
      } else {
        return std::nullopt;
      }
    }

    result.push_back(std::move(current_group));
  }

  return result;
}
/* NOLINTEND(*swappable-parameters,*mt-unsafe,*pointer-arithmetic) */
} // namespace river
