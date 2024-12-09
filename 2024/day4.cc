/*
"Looks like the Chief's not here. Next!" One of The Historians pulls out a
device and pushes the only button on it. After a brief flash, you recognize the
interior of the Ceres monitoring station!

As the search for the Chief continues, a small Elf who lives on the station tugs
on your shirt; she'd like to know if you could help her with her word search
(your puzzle input). She only has to find one word: XMAS.

This word search allows words to be horizontal, vertical, diagonal, written
backwards, or even overlapping other words. It's a little unusual, though, as
you don't merely need to find one instance of XMAS - you need to find all of
them. Here are a few ways XMAS might appear, where irrelevant characters have
been replaced with .:


..X...
.SAMX.
.A..A.
XMAS.S
.X....
The actual word search will be full of letters instead. For example:

MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX
In this word search, XMAS occurs a total of 18 times; here's the same word
search again, but where letters not involved in any XMAS have been replaced with
.:

....XXMAS.
.SAMXMS...
...S..A...
..A.A.MS.X
XMASAMX.MM
X.....XA.A
S.S.S.S.SS
.A.A.A.A.A
..M.M.M.MM
.X.X.XMASX
Take a look at the little Elf's word search. How many times does XMAS appear?

--- Part Two ---
The Elf looks quizzically at you. Did you misunderstand the assignment?

Looking for the instructions, you flip over the word search to find that this
isn't actually an XMAS puzzle; it's an X-MAS puzzle in which you're supposed to
find two MAS in the shape of an X. One way to achieve that is like this:

M.S
.A.
M.S
Irrelevant characters have again been replaced with . in the above diagram.
Within the X, each MAS can be written forwards or backwards.

Here's the same example from before, but this time all of the X-MASes have been
kept instead:

.M.S......
..A..MSMS.
.M.S.MAA..
..A.ASMSM.
.M.S.M....
..........
S.S.S.S.S.
.A.A.A.A..
M.M.M.M.M.
..........
In this example, an X-MAS appears 9 times.

Flip the word search from the instructions back over to the word search side and
try again. How many times does an X-MAS appear?

*/
#include <cstdlib>

#include "absl/log/check.h"
#include <print>
#include "utils/utils.h"

struct Coordinate {
  int x = 0;
  int y = 0;
};

bool ValidateCoordinate(Coordinate coordinate, int rows, int cols) {
  auto [x, y] = coordinate;
  // Just validate that the coordinate is within bounds.
  return x >= 0 && y >= 0 && x < cols && y < rows;
}

// Check for MAS with the provided increment instructions.
bool CheckForMas(const std::vector<std::string>& grid, Coordinate current,
                 int x_step, int y_step) {
  const int rows = grid.size();
  const int cols = grid.front().size();
  auto IsInvalid = [rows, cols](Coordinate coordinate) {
    return !ValidateCoordinate(coordinate, rows, cols);
  };
  for (char c : std::string("MAS")) {
    current.x += x_step;
    current.y += y_step;
    // std::print("Checking for {} at row {}, col {}. Invalid? {}\n", c,
    // current.y, current.x, IsInvalid(current));
    if (IsInvalid(current) || grid[current.y][current.x] != c) {
      return false;
    }
  }
  return true;
}

// There are up to 8 different XMAS from each X.
// This method checks how many different directions result in XMAS.
int32_t CountXmasFromX(const std::vector<std::string>& grid, Coordinate start) {
  auto [x, y] = start;
  // Check if this is a valid starting point.
  if (grid[y][x] != 'X') {
    return 0;
  }

  // std::print("Found an X at row {}, col {}\n", start.y, start.x);

  int32_t count = 0;

  for (int x_step : {-1, 0, 1}) {
    for (int y_step : {-1, 0, 1}) {
      if (x_step == 0 && y_step == 0) continue;
      if (CheckForMas(grid, start, x_step, y_step)) {
        ++count;
      }
    }
  }
  return count;
}

// Returns true if this is a valid MAS in an X shape.
bool CountMasInAnXShape(const std::vector<std::string>& grid,
                        Coordinate start) {
  auto [x, y] = start;
  // Check if this is a valid starting point.
  if (grid[y][x] != 'A') {
    return 0;
  }
  const int rows = grid.size();
  const int cols = grid.front().size();
  auto IsInvalid = [rows, cols](Coordinate coordinate) {
    return !ValidateCoordinate(coordinate, rows, cols);
  };

  Coordinate bottom_left = {x + 1, y - 1};
  Coordinate bottom_right = {x + 1, y + 1};
  Coordinate top_left = {x - 1, y - 1};
  Coordinate top_right = {x - 1, y + 1};
  if (IsInvalid(bottom_left) || IsInvalid(bottom_right) ||
      IsInvalid(top_left) || IsInvalid(top_right)) {
    // Any invalid coordinates disqualify.
    return false;
  }

  // Little lambda for brevity.
  auto Get = [&grid](Coordinate c) { return grid[c.y][c.x]; };

  // Check bottom left and top right are valid.
  if (!((Get(bottom_left) == 'M' && Get(top_right) == 'S') ||
        (Get(bottom_left) == 'S' && Get(top_right) == 'M'))) {
    return false;
  }
  // Now check top_left and bottom_right.
  if (!((Get(top_left) == 'M' && Get(bottom_right) == 'S') ||
        (Get(top_left) == 'S' && Get(bottom_right) == 'M'))) {
    return false;
  }
  return true;
}

int main() {
  std::vector<std::string> grid =
      aoc::LoadStringsFromFileByLine("./2024/day4.txt");

  int32_t total{0};
  int32_t total_part2{0};
  for (int y{0}; y < grid.size(); ++y) {
    for (int x{0}; x < grid.front().size(); ++x) {
      total += CountXmasFromX(grid, {x, y});
      total_part2 += CountMasInAnXShape(grid, {x, y});
    }
  }
  std::print("The total is: {}\n", total);
  std::print("The part 2 total is: {}\n", total_part2);
}
