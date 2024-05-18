#include <numeric>
#include <string>
#include <vector>

namespace aoc {

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

// Strip leading and trailing whitespace from all strings in supplied vec.
void StripWhitespace(std::vector<std::string>& vec);

// Read the file by file path into a std::string.
std::string ReadFileToString(const std::string& file_path);

// Read the file by file path, returning each line as a std::string in a
// std::vector.
std::vector<std::string> LoadStringsFromFileByLine(
    const std::string& file_path);

}  // namespace aoc
