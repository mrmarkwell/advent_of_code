#pragma once
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "absl/status/statusor.h"
#include "fmt/core.h"

namespace aoc {

// Quick macro to print a variable when debugging.
// Usage: PRINT(foo);
// #define PRINT(var) fmt::print(#var ": {}\n", var)

// // Hashable, equality comparable Coordinate.
// // This coordinate denotes rows and columns rather than x and y. The meaning
// of
// 'row' and 'col' can be determined by the client code.
struct Coordinate {
  int row{0};
  int col{0};

  bool operator==(const Coordinate& other) const {
    return row == other.row && col == other.col;
  }
  template <typename H>
  friend H AbslHashValue(H h, const Coordinate& m);
};

// template <typename H>
// H AbslHashValue(H h, const Coordinate& m) {
//   return H::combine(std::move(h), m.row, m.col);
// }

// Trait to detect if a type is a container
// template <typename, typename = void>
// struct is_container : std::false_type {};

// template <typename T>
// struct is_container<T, std::void_t<decltype(std::begin(std::declval<T&>())),
//                                    decltype(std::end(std::declval<T&>())),
//                                    typename T::value_type>> : std::true_type
//                                    {};

// template <typename T>
// inline constexpr bool is_container_v = is_container<T>::value;

// Contains function template
// template <typename Container, typename T>
// std::enable_if_t<is_container_v<Container>, bool> Contains(const Container&
// c,
//                                                            const T& value) {
//   using element_type = typename Container::value_type;
//   if constexpr (is_container_v<element_type>) {
//     // Element type is a container, recurse into each element
//     for (const auto& inner : c) {
//       if (Contains(inner, value)) {
//         return true;
//       }
//     }
//     return false;
//   } else {
//     // Element type is not a container, use std::find directly
//     return std::find(std::begin(c), std::end(c), value) != std::end(c);
//   }
// }

// template <typename T>
// void PrettyPrintVector(const std::vector<T>& vec) {
//   fmt::print("[");
//   for (size_t i = 0; i < vec.size(); ++i) {
//     fmt::print("{}", vec[i]);
//     if (i < vec.size() - 1) {
//       fmt::print(", ");
//     }
//   }
//   fmt::print("]\n");
// }

// Kind of goofy, so here is an explanation.
// - This is a generic lambda  a lambda that takes a reference to a deduced type
// parameter. In this case it must be a container of containers or the code
// won't build.
// - The return type is explititly set to size_t.
// - This is a variable defined in a header file, so to avoid ODR violations, it
// must be declared `inline`.
// - The type is `auto` since lambda types are ugly and hard to reason about.
// - This could have been a regular function that takes
// std::vector<std::string>, but then it would only work for that type.
// - Alternatively, this could have been a traditional function template, but I
// think these generic lambdas are a more elegant way to express generic
// functors.
inline auto TotalSize = [](const auto& container_of_containers) -> size_t {
  return std::accumulate(
      container_of_containers.begin(), container_of_containers.end(), 0,
      [](size_t sum, const std::string& str) { return sum + str.size(); });
};

// Trim leading and trailing whitespace from a string.
std::string Trim(const std::string& str);
std::string_view TrimWhitespace(std::string_view str);

// Strip leading and trailing whitespace from all strings in supplied vec.
void StripWhitespace(std::vector<std::string>& vec);

// Read the file by file path into a std::string.
std::string ReadFileToString(const std::string& file_path);

// Utility function to parse input data based on a given regex pattern and
// processing function.
template <typename T>
std::vector<T> ParseDataWithRegex(
    const std::vector<std::string>& lines, const std::regex& pattern,
    std::function<T(const std::vector<std::string>&)> ProcessMatchFunction) {
  std::vector<T> results;

  for (const auto& line : lines) {
    std::smatch matches;
    if (std::regex_search(line, matches, pattern)) {
      // Extract all matches except the first one.
      std::vector<std::string> captured_groups;
      for (size_t i = 1; i < matches.size(); ++i) {
        captured_groups.push_back(matches[i].str());
      }
      // Process the captured groups
      results.push_back(ProcessMatchFunction(captured_groups));
    }
  }

  return results;
};

// Read the file by file path, returning each line as a std::string in a
// std::vector.
std::vector<std::string> LoadStringsFromFileByLine(
    const std::string& file_path);

std::vector<std::string> SplitDelimitedString(std::string_view input,
                                              std::string_view delimiter = "");

// Converts an input string into an integer or returns a failing status.
absl::StatusOr<int64_t> ConvertStringViewToInt64(std::string_view input);

}  // namespace aoc
