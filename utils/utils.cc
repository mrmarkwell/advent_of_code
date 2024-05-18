#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

}  // namespace aoc
