#include <string>
#include <vector>

namespace aoc {

// Read the file (given by filename) into a std::string.
std::string ReadFileToString(const std::string& filename);
std::vector<std::string> LoadStringsFromFileByLine(
    const std::string& file_path);

}  // namespace aoc
