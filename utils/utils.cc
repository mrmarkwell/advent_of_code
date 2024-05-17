#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

std::string ReadFileToString(const std::string& file_path) {
  std::ifstream file_stream(file_path, std::ios::in | std::ios::binary);
  assert(file_stream && "Could not open file");

  std::ostringstream buffer;
  buffer << file_stream.rdbuf();

  return buffer.str();
}

std::vector<std::string> LoadStringsFromFile(const std::string& file_path) {
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