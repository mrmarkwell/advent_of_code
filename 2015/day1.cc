#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "absl/strings/str_join.h"
#include "advent_of_code/utils/utils.h"

struct FloorInfo {
  int final_floor = 0;
  std::optional<int> first_basement_position = std::nullopt;
};

FloorInfo CountFloors(std::string_view input) {
  FloorInfo info;
  int position = 0;

  for (const char c : input) {
    // Always increment the position counter.
    ++position;
    if (c == '(') ++info.final_floor;
    if (c == ')') --info.final_floor;
    if (info.final_floor < 0 && !info.first_basement_position) {
      // We've entered the basement for the first time.
      info.first_basement_position = position;
    }
  }
  return info;
}

int main() {
  std::string input = ReadFileToString("./advent_of_code/2015/day1.txt");
  std::vector<std::string> v = {"foo", "bar", "baz"};
  std::string s = absl::StrJoin(v, "-");

  std::cout << "Joined string: " << s << "\n";

  assert(CountFloors("(())").final_floor == 0);
  assert(CountFloors("(()))").first_basement_position == 5);
  assert(CountFloors("()()").final_floor == 0);
  assert(CountFloors("()()").first_basement_position == std::nullopt);
  assert(CountFloors("(((").final_floor == 3);
  assert(CountFloors("(()(()(").final_floor == 3);
  assert(CountFloors(")())())").final_floor == -3);
  assert(CountFloors(")())())").first_basement_position == 1);

  FloorInfo info = CountFloors(input);
  std::cout << "floor = " << info.final_floor << std::endl;
  if (info.first_basement_position) {
    std::cout << "first basement position = " << *info.first_basement_position
              << std::endl;
  }
  return 0;
}
