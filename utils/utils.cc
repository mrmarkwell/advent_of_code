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

// Splits a comma-delimited string into a vector of trimmed substrings.
std::vector<std::string> SplitCommaDelimitedString(std::string_view input) {
  std::vector<std::string> result;

  while (!input.empty()) {
    // Find the next comma.
    size_t pos = input.find(',');

    // Extract the substring before the comma.
    std::string_view token = input.substr(0, pos);

    // Trim whitespace and convert to std::string before adding to the result.
    result.emplace_back(std::string(Trim(token)));

    // If no comma was found, we're done.
    if (pos == std::string_view::npos) {
      break;
    }

    // Move to the substring after the comma.
    input.remove_prefix(pos + 1);
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
