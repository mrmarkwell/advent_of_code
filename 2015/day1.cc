/*

--- Day 1: Not Quite Lisp ---
Santa was hoping for a white Christmas, but his weather machine's "snow"
function is powered by stars, and he's fresh out! To save Christmas, he needs
you to collect fifty stars by December 25th.

Collect stars by helping Santa solve puzzles. Two puzzles will be made available
on each day in the Advent calendar; the second puzzle is unlocked when you
complete the first. Each puzzle grants one star. Good luck!

Here's an easy puzzle to warm you up.

Santa is trying to deliver presents in a large apartment building, but he can't
find the right floor - the directions he got are a little confusing. He starts
on the ground floor (floor 0) and then follows the instructions one character at
a time.

An opening parenthesis, (, means he should go up one floor, and a closing
parenthesis, ), means he should go down one floor.

The apartment building is very tall, and the basement is very deep; he will
never find the top or bottom floors.

For example:

(()) and ()() both result in floor 0.
((( and (()(()( both result in floor 3.
))((((( also results in floor 3.
()) and ))( both result in floor -1 (the first basement level).
))) and )())()) both result in floor -3.
To what floor do the instructions take Santa?

Your puzzle answer was 74.

--- Part Two ---
Now, given the same instructions, find the position of the first character that
causes him to enter the basement (floor -1). The first character in the
instructions has position 1, the second character has position 2, and so on.

For example:

) causes him to enter the basement at character position 1.
()()) causes him to enter the basement at character position 5.
What is the position of the character that causes Santa to first enter the
basement?

Your puzzle answer was 1795.

Both parts of this puzzle are complete! They provide two gold stars: **
*/

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "absl/strings/str_join.h"
#include "utils/utils.h"

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
  std::string input = ReadFileToString("./2015/day1.txt");
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
