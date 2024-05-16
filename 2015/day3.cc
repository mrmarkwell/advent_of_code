/*
--- Day 3: Perfectly Spherical Houses in a Vacuum ---
Santa is delivering presents to an infinite two-dimensional grid of houses.

He begins by delivering a present to the house at his starting location, and
then an elf at the North Pole calls him via radio and tells him where to move
next. Moves are always exactly one house to the north (^), south (v), east (>),
or west (<). After each move, he delivers another present to the house at his
new location.

However, the elf back at the north pole has had a little too much eggnog, and so
his directions are a little off, and Santa ends up visiting some houses more
than once. How many houses receive at least one present?

For example:

- > delivers presents to 2 houses: one at the starting location, and one to the
east.
- ^>v< delivers presents to 4 houses in a square, including twice to the house
at his starting/ending location.
- ^v^v^v^v^v delivers a bunch of presents to some very lucky children at only 2
houses.

Your puzzle answer was 2565.

The first half of this puzzle is complete! It provides one gold star: *

--- Part Two ---
The next year, to speed up the process, Santa creates a robot version of
himself, Robo-Santa, to deliver presents with him.

Santa and Robo-Santa start at the same location (delivering two presents to the
same starting house), then take turns moving based on instructions from the elf,
who is eggnoggedly reading from the same script as the previous year.

This year, how many houses receive at least one present?

For example:

- ^v delivers presents to 3 houses, because Santa goes north, and then
Robo-Santa goes south.
- ^>v< now delivers presents to 3 houses, and Santa and Robo-Santa end up back
where they started.
- ^v^v^v^v^v now delivers presents to 11 houses, with Santa going one direction
and Robo-Santa going the other.


*/

#include "absl/container/flat_hash_set.h"
#include "utils/utils.h"

// Define a struct to represent a coordinate
struct Coordinate {
  int x = 0;
  int y = 0;

  // Define the equality operator for the Coordinate
  bool operator==(const Coordinate &other) const {
    return x == other.x && y == other.y;
  }
};

// Define a hash function for the Coordinate
struct CoordinateHash {
  std::size_t operator()(const Coordinate &coord) const {
    // Combine the hash of x and y
    return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.y) << 1);
  }
};

absl::flat_hash_set<Coordinate, CoordinateHash> ComputeVisitedHouseSet(
    std::string_view input) {
  absl::flat_hash_set<Coordinate, CoordinateHash> visited_homes;

  // Track our current coordinate. Start at 0, 0.
  Coordinate current = {0, 0};

  // Santa visits the first home by default.
  visited_homes.insert(current);
  for (const char c : input) {
    switch (c) {
      case '^': {
        ++current.y;
        break;
      }
      case 'v': {
        --current.y;
        break;
      }
      case '>': {
        ++current.x;
        break;
      }
      case '<': {
        --current.x;
        break;
      }
      default: {
        std::cout << "Unexpected character: " << c << "\n";
      }
    }
    // Regardless of what happens, add the visited home to the set.
    visited_homes.insert(current);
  }

  return visited_homes;
}

struct Instructions {
  std::string santa_instructions;
  std::string robo_instructions;
};

Instructions ParseInstructions(std::string_view input) {
  // Split the original instructions into instructions for Santa and for
  // RoboSanta.
  Instructions result;
  result.santa_instructions.reserve((input.size() + 1) / 2);
  result.robo_instructions.reserve((input.size() + 1) / 2);

  bool santas_turn = true;
  for (const char c : input) {
    if (santas_turn) {
      result.santa_instructions += c;
    } else {
      result.robo_instructions += c;
    }
    santas_turn = !santas_turn;
  }
  return result;
}

absl::flat_hash_set<Coordinate, CoordinateHash> ComputeVisitedHouseSetWithRobo(
    std::string_view input) {
  Instructions instructions = ParseInstructions(input);
  auto visited_houses = ComputeVisitedHouseSet(instructions.santa_instructions);
  visited_houses.merge(ComputeVisitedHouseSet(instructions.robo_instructions));
  return visited_houses;
}

int main() {
  std::string input = ReadFileToString("./2015/day3.txt");

  assert(ComputeVisitedHouseSet("^").size() == 2);
  assert(ComputeVisitedHouseSet("^>v<").size() == 4);
  assert(ComputeVisitedHouseSet("^v^v^v^v^v").size() == 2);

  std::cout << "Number of visited homes: "
            << ComputeVisitedHouseSet(input).size() << "\n";

  assert(ComputeVisitedHouseSetWithRobo("^v").size() == 3);
  assert(ComputeVisitedHouseSetWithRobo("^>v<").size() == 3);
  assert(ComputeVisitedHouseSetWithRobo("^v^v^v^v^v").size() == 11);

  std::cout << "Number of visited homes with robo: "
            << ComputeVisitedHouseSetWithRobo(input).size() << "\n";
  return 0;
}
