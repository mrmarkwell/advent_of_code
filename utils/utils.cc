#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "absl/status/statusor.h"

namespace aoc {
// Function to strip leading and trailing whitespace from a single string
std::string Trim(const std::string& str) {
  auto front = std::find_if_not(str.begin(), str.end(),
                                [](int c) { return std::isspace(c); });
  auto back = std::find_if_not(str.rbegin(), str.rend(), [](int c) {
                return std::isspace(c);
              }).base();
  if (front < back) {
    return std::string(front, back);
  } else {
    return std::string();
  }
}

// Helper function to strip leading and trailing whitespace from each string in
// the vector
void StripWhitespace(std::vector<std::string>& vec) {
  for (auto& str : vec) {
    str = Trim(str);
  }
}
std::string ReadFileToString(const std::string& file_path) {
  std::ifstream file_stream(file_path, std::ios::in | std::ios::binary);
  assert(file_stream && "Could not open file");

  std::ostringstream buffer;
  buffer << file_stream.rdbuf();

  return buffer.str();
}

std::vector<std::string> LoadStringsFromFileByLine(
    const std::string& file_path) {
  std::vector<std::string> str_vector = {};
  std::ifstream file;
  file.open(file_path, std::ios::in);
  if (!file.is_open()) {
    std::cout << "Failed to open file " << file_path << "\n";
    return {};
  }
  std::string str;
  std::string file_contents;
  while (std::getline(file, str)) {
    str_vector.push_back(str);
  }
  return str_vector;
}
// Helper function to trim leading and trailing whitespace from a string_view.
std::string_view Trim(std::string_view str) {
  auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
               return std::isspace(ch);
             }).base();

  // If the string is entirely whitespace, return an empty string_view.
  if (start >= end) {
    return std::string_view();
  }

  return std::string_view(&(*start), std::distance(start, end));
}

// Splits a delimited string into a vector of trimmed substrings.
std::vector<std::string> SplitDelimitedString(std::string_view input,
                                              std::string_view delimiter = "") {
  std::vector<std::string> result;

  if (delimiter.empty()) {
    // Split by whitespace when delimiter is empty.
    size_t start = 0;
    while (start < input.size()) {
      // Find the start of the next word.
      start = input.find_first_not_of(" \t\n\r", start);
      if (start == std::string_view::npos) {
        break;
      }

      // Find the end of the current word.
      size_t end = input.find_first_of(" \t\n\r", start);
      result.emplace_back(std::string(Trim(input.substr(start, end - start))));

      // Move to the next part of the string.
      start = end;
    }
  } else {
    // Split by the specified delimiter.
    while (!input.empty()) {
      size_t pos = input.find(delimiter);

      // Extract the substring before the delimiter.
      std::string_view token = input.substr(0, pos);
      result.emplace_back(std::string(Trim(token)));

      // If no delimiter was found, we're done.
      if (pos == std::string_view::npos) {
        break;
      }

      // Move to the substring after the delimiter.
      input.remove_prefix(pos + delimiter.size());
    }
  }

  return result;
}

// Function to convert the entire string into an int64_t
absl::StatusOr<int64_t> ConvertStringViewToInt64(std::string_view input) {
  // Handle the case where the string is empty
  if (input.empty()) {
    return absl::InvalidArgumentError("Input string is empty.");
  }

  // Parse the entire string into an int64_t using std::from_chars
  int64_t result = 0;
  auto [ptr, ec] = std::from_chars(input.begin(), input.end(), result);

  // Handle errors during conversion
  if (ec == std::errc::invalid_argument) {
    return absl::InvalidArgumentError(
        "The input string contains invalid characters.");
  }
  if (ec == std::errc::result_out_of_range) {
    return absl::OutOfRangeError("The number is out of range for an int64_t.");
  }

  return result;
}

}  // namespace aoc
