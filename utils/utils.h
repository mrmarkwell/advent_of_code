#pragma once
#include <format>
#include <numeric>
#include <print>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "absl/status/statusor.h"

namespace aoc {

enum class Direction {
  kUp,
  kDown,
  kLeft,
  kRight,
};

// Quick macro to print a variable when debugging.
// Usage: PRINT(foo);
#define PRINT(var) std::print(#var ": {}\n", var)
// // Hashable, equality comparable Coordinate.
// // This coordinate denotes rows and columns rather than x and y. The meaning
// of
// 'row' and 'col' can be determined by the client code.
struct Coordinate {
  int row{0};
  int64_t col{0};

  bool operator==(const Coordinate& other) const {
    return row == other.row && col == other.col;
  }

  // Addition operator
  Coordinate operator+(const Coordinate& other) const {
    return {row + other.row, col + other.col};
  }

  // Subtraction operator
  Coordinate operator-(const Coordinate& other) const {
    return {row - other.row, col - other.col};
  }

  // Addition assignment operator
  Coordinate& operator+=(const Coordinate& other) {
    row += other.row;
    col += other.col;
    return *this;
  }

  // Subtraction assignment operator
  Coordinate& operator-=(const Coordinate& other) {
    row -= other.row;
    col -= other.col;
    return *this;
  }

  std::string ToString() const { return std::format("{{{}, {}}}", row, col); }
  void Print() const { std::print("{}\n", ToString()); }

  template <typename H>
  friend H AbslHashValue(H h, const Coordinate& m);
};

template <typename H>
H AbslHashValue(H h, const Coordinate& m) {
  return H::combine(std::move(h), m.row, m.col);
}

// Convert a char to an int64_t. WARNING: No error checking.
int64_t ConvertCharToInt(char c);

class VisitedMap {
 public:
  // Constructor to initialize the map with given dimensions.
  // Defaults to all cells unvisited (false).
  VisitedMap(int rows, int cols)
      : visited_(rows, std::vector<bool>(cols, false)) {}

  // Marks a cell as visited.
  bool MarkVisited(Coordinate c) {
    if (IsOutOfBounds(c)) {
      return false;
    }
    visited_[c.row][c.col] = true;
    return true;
  }

  // Marks a cell as unvisited.
  bool MarkUnvisited(Coordinate c) {
    if (IsOutOfBounds(c)) {
      return false;
    }
    visited_[c.row][c.col] = false;
    return true;
  }

  // Checks if a cell has been visited.
  bool IsVisited(Coordinate c) const {
    if (IsOutOfBounds(c)) {
      return false;
    }
    return visited_[c.row][c.col];
  }

  // Resets the entire map to unvisited.
  void Reset() {
    for (auto& row : visited_) {
      std::fill(row.begin(), row.end(), false);
    }
  }

  // Gets the number of rows in the map.
  int NumRows() const { return static_cast<int>(visited_.size()); }

  // Gets the number of columns in the map.
  int NumCols() const {
    return NumRows() > 0 ? static_cast<int>(visited_[0].size()) : 0;
  }

  // Checks if the given cell is out of bounds.
  bool IsOutOfBounds(Coordinate c) const {
    return c.row < 0 || c.row >= NumRows() || c.col < 0 || c.col >= NumCols();
  }

 private:
  std::vector<std::vector<bool>>
      visited_;  // 2D grid to store visitation states.
};

// A rectangular grid of strings, with some helper functions.
class Map {
 public:
  explicit Map(std::vector<std::string> map) : map_(std::move(map)) {}

  void Print() {
    std::print("\n");
    for (std::string_view row : map_) {
      std::print("{}\n", row);
    }
    std::print("\n");
  }

  bool IsOutOfBounds(Coordinate c) const {
    bool in_bounds =
        c.row >= 0 && c.row < NumRows() && c.col >= 0 && c.col < NumCols();

    return !in_bounds;
  }

  char GetChar(Coordinate c) const { return map_[c.row][c.col]; }
  void SetChar(Coordinate c, char val) { map_[c.row][c.col] = val; }
  bool CharEquals(Coordinate c, char val) const {
    return !IsOutOfBounds(c) && map_[c.row][c.col] == val;
  }
  // Only do this if the char is an int64_t.
  int64_t GetInt(Coordinate c) const {
    return ::aoc::ConvertCharToInt(GetChar(c));
  }
  int64_t NumRows() const { return map_.size(); }
  int64_t NumCols() const { return map_.front().size(); }

 private:
  std::vector<std::string> map_;
};

// Trait to detect if a type is a container
template <typename, typename = void>
struct is_container : std::false_type {};

template <typename T>
struct is_container<T, std::void_t<decltype(std::begin(std::declval<T&>())),
                                   decltype(std::end(std::declval<T&>())),
                                   typename T::value_type>> : std::true_type {};

template <typename T>
inline constexpr bool is_container_v = is_container<T>::value;

// Contains function template
template <typename Container, typename T>
std::enable_if_t<is_container_v<Container>, bool> Contains(const Container& c,
                                                           const T& value) {
  using element_type = typename Container::value_type;
  if constexpr (is_container_v<element_type>) {
    // Element type is a container, recurse into each element
    for (const auto& inner : c) {
      if (Contains(inner, value)) {
        return true;
      }
    }
    return false;
  } else {
    // Element type is not a container, use std::find directly
    return std::find(std::begin(c), std::end(c), value) != std::end(c);
  }
}

inline Coordinate GoDown(Coordinate c) {
  ++c.row;
  return c;
}
inline Coordinate GoUp(Coordinate c) {
  --c.row;
  return c;
}
inline Coordinate GoLeft(Coordinate c) {
  --c.col;
  return c;
}
inline Coordinate GoRight(Coordinate c) {
  ++c.col;
  return c;
}

template <typename T>
void PrettyPrintVector(const std::vector<T>& vec) {
  std::print("[");
  for (size_t i = 0; i < vec.size(); ++i) {
    std::print("{}", vec[i]);
    if (i < vec.size() - 1) {
      std::print(", ");
    }
  }
  std::print("]\n");
}

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
