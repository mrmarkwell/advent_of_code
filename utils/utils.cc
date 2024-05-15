#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

std::string ReadFileToString(const std::string& file_path) {
  std::ifstream file_stream(file_path, std::ios::in | std::ios::binary);
  assert(file_stream && "Could not open file");

  std::ostringstream buffer;
  buffer << file_stream.rdbuf();

  return buffer.str();
}
