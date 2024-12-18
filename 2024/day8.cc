/*
--- Day 8: Resonant Collinearity ---
You find yourselves on the roof of a top-secret Easter Bunny installation.

While The Historians do their thing, you take a look at the familiar huge
antenna. Much to your surprise, it seems to have been reconfigured to emit a
signal that makes people 0.1% more likely to buy Easter Bunny brand Imitation
Mediocre Chocolate as a Christmas gift! Unthinkable!

Scanning across the city, you find that there are actually many such antennas.
Each antenna is tuned to a specific frequency indicated by a single lowercase
letter, uppercase letter, or digit. You create a map (your puzzle input) of
these antennas. For example:

............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............
The signal only applies its nefarious effect at specific antinodes based on the
resonant frequencies of the antennas. In particular, an antinode occurs at any
point that is perfectly in line with two antennas of the same frequency - but
only when one of the antennas is twice as far away as the other. This means that
for any pair of antennas with the same frequency, there are two antinodes, one
on either side of them.

So, for these two antennas with frequency a, they create the two antinodes
marked with #:

..........
...#......
..........
....a.....
..........
.....a....
..........
......#...
..........
..........
Adding a third antenna with the same frequency creates several more antinodes.
It would ideally add four antinodes, but two are off the right side of the map,
so instead it adds only two:

..........
...#......
#.........
....a.....
........a.
.....a....
..#.......
......#...
..........
..........
Antennas with different frequencies don't create antinodes; A and a count as
different frequencies. However, antinodes can occur at locations that contain
antennas. In this diagram, the lone antenna with frequency capital A creates no
antinodes but has a lowercase-a-frequency antinode at its location:

..........
...#......
#.........
....a.....
........a.
.....a....
..#.......
......A...
..........
..........
The first example has antennas with two different frequencies, so the antinodes
they create look like this, plus an antinode overlapping the topmost A-frequency
antenna:

......#....#
...#....0...
....#0....#.
..#....0....
....0....#..
.#....A.....
...#........
#......#....
........A...
.........A..
..........#.
..........#.
Because the topmost A-frequency antenna overlaps with a 0-frequency antinode,
there are 14 total unique locations that contain an antinode within the bounds
of the map.

Calculate the impact of the signal. How many unique locations within the bounds
of the map contain an antinode?

Your puzzle answer was 357.

The first half of this puzzle is complete! It provides one gold star: *

--- Part Two ---
Watching over your shoulder as you work, one of The Historians asks if you took
the effects of resonant harmonics into your calculations.

Whoops!

After updating your model, it turns out that an antinode occurs at any grid
position exactly in line with at least two antennas of the same frequency,
regardless of distance. This means that some of the new antinodes will occur at
the position of each antenna (unless that antenna is the only one of its
frequency).

So, these three T-frequency antennas now create many antinodes:

T....#....
...T......
.T....#...
.........#
..#.......
..........
...#......
..........
....#.....
..........
In fact, the three T-frequency antennas are all exactly in line with two
antennas, so they are all also antinodes! This brings the total number of
antinodes in the above example to 9.

The original example now has 34 antinodes, including the antinodes that appear
on every antenna:

##....#....#
.#.#....0...
..#.#0....#.
..##...0....
....0....#..
.#...#A....#
...#..#.....
#....#.#....
..#.....A...
....#....A..
.#........#.
...#......##
Calculate the impact of the signal using this updated model. How many unique
locations within the bounds of the map contain an antinode?
 */
#include <cstdlib>
#include <print>

#include "absl/container/flat_hash_set.h"
#include "utils/utils.h"

using ::aoc::Coordinate;

class Map {
 public:
  static constexpr char kEmpty = '.';
  static constexpr char kAntinode = '#';
  explicit Map(std::vector<std::string> map) : map_(std::move(map)) {}

  absl::flat_hash_set<Coordinate> ComputeAntinodes(bool with_harmonics) {
    absl::flat_hash_set<Coordinate> antinodes;

    for (int row = 0; row < NumRows(); ++row) {
      for (int col = 0; col < NumCols(); ++col) {
        Coordinate position{row, col};
        if (IsEmpty(position)) {
          continue;
        }
        // We have an antenna.
        antinodes.merge(ComputeCoordinateAntinodes(position, with_harmonics));
      }
    }

    return antinodes;
  }

  absl::flat_hash_set<Coordinate> ComputeCoordinateAntinodes(
      Coordinate antenna, bool with_harmonics) {
    absl::flat_hash_set<Coordinate> antinodes;
    char antenna_type = GetChar(antenna);
    for (int row = 0; row < NumRows(); ++row) {
      for (int col = 0; col < NumCols(); ++col) {
        Coordinate position{row, col};
        if (antenna == position) {
          //  Skip current position.
          continue;
        }
        if (GetChar(position) == antenna_type) {
          // We found a matching position.
          Coordinate delta = position - antenna;

          Coordinate candidate = position;

          if (with_harmonics) {
            antinodes.insert(candidate);
          }

          while (true) {
            candidate += delta;
            if (IsOutOfBounds(candidate)) {
              break;
            }
            antinodes.insert(candidate);
            if (!with_harmonics) {
              break;
            }
          }
        }
      }
    }
    return antinodes;
  }

  void Print() {
    std::print("\n");
    for (std::string_view row : map_) {
      std::print("{}\n", row);
    }
    std::print("\n");
  }

  bool IsOutOfBounds(Coordinate c) const {
    bool in_bounds =
        c.row >= 0 && c.row < NumRows() && c.col >= 0 && c.col < NumCols();

    return !in_bounds;
  }

  char GetChar(Coordinate c) const { return map_[c.row][c.col]; }
  bool IsEmpty(Coordinate c) const {
    return GetChar(c) == kEmpty || GetChar(c) == kAntinode;
  }
  int NumRows() const { return map_.size(); }
  int NumCols() const { return map_.front().size(); }

 private:
  std::vector<std::string> map_;
};

int main() {
  Map map{aoc::LoadStringsFromFileByLine("./2024/day8.txt")};

  // Keep a set of antinode coordinates.
  absl::flat_hash_set<Coordinate> antinodes =
      map.ComputeAntinodes(/*with_harmonics=*/false);
  absl::flat_hash_set<Coordinate> harmonic_antinodes =
      map.ComputeAntinodes(/*with_harmonics=*/true);

  map.Print();
  std::print("There are {} antinodes.\n", antinodes.size());
  std::print("There are {} harmonic antinodes.\n", harmonic_antinodes.size());

  return 0;
}
