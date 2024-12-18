/*
--- Day 10: Hoof It ---
You all arrive at a Lava Production Facility on a floating island in the sky. As
the others begin to search the massive industrial complex, you feel a small nose
boop your leg and look down to discover a reindeer wearing a hard hat.

The reindeer is holding a book titled "Lava Island Hiking Guide". However, when
you open the book, you discover that most of it seems to have been scorched by
lava! As you're about to ask how you can help, the reindeer brings you a blank
topographic map of the surrounding area (your puzzle input) and looks up at you
excitedly.

Perhaps you can help fill in the missing hiking trails?

The topographic map indicates the height at each position using a scale from 0
(lowest) to 9 (highest). For example:

0123
1234
8765
9876
Based on un-scorched scraps of the book, you determine that a good hiking trail
is as long as possible and has an even, gradual, uphill slope. For all practical
purposes, this means that a hiking trail is any path that starts at height 0,
ends at height 9, and always increases by a height of exactly 1 at each step.
Hiking trails never include diagonal steps - only up, down, left, or right (from
the perspective of the map).

You look up from the map and notice that the reindeer has helpfully begun to
construct a small pile of pencils, markers, rulers, compasses, stickers, and
other equipment you might need to update the map with hiking trails.

A trailhead is any position that starts one or more hiking trails - here, these
positions will always have height 0. Assembling more fragments of pages, you
establish that a trailhead's score is the number of 9-height positions reachable
from that trailhead via a hiking trail. In the above example, the single
trailhead in the top left corner has a score of 1 because it can reach a single
9 (the one in the bottom left).

This trailhead has a score of 2:

...0...
...1...
...2...
6543456
7.....7
8.....8
9.....9
(The positions marked . are impassable tiles to simplify these examples; they do
not appear on your actual topographic map.)

This trailhead has a score of 4 because every 9 is reachable via a hiking trail
except the one immediately to the left of the trailhead:

..90..9
...1.98
...2..7
6543456
765.987
876....
987....
This topographic map contains two trailheads; the trailhead at the top has a
score of 1, while the trailhead at the bottom has a score of 2:

10..9..
2...8..
3...7..
4567654
...8..3
...9..2
.....01
Here's a larger example:

89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
This larger example has 9 trailheads. Considering the trailheads in reading
order, they have scores of 5, 6, 5, 3, 1, 3, 5, 3, and 5. Adding these scores
together, the sum of the scores of all trailheads is 36.

The reindeer gleefully carries over a protractor and adds it to the pile. What
is the sum of the scores of all trailheads on your topographic map?

--- Part Two ---

The reindeer spends a few minutes reviewing your hiking trail map before
realizing something, disappearing for a few minutes, and finally returning with
yet another slightly-charred piece of paper.

The paper describes a second way to measure a trailhead called its rating. A
trailhead's rating is the number of distinct hiking trails which begin at that
trailhead. For example:

.....0.
..4321.
..5..2.
..6543.
..7..4.
..8765.
..9....
The above map has a single trailhead; its rating is 3 because there are exactly
three distinct hiking trails which begin at that position:

.....0.   .....0.   .....0.
..4321.   .....1.   .....1.
..5....   .....2.   .....2.
..6....   ..6543.   .....3.
..7....   ..7....   .....4.
..8....   ..8....   ..8765.
..9....   ..9....   ..9....
Here is a map containing a single trailhead with rating 13:

..90..9
...1.98
...2..7
6543456
765.987
876....
987....
This map contains a single trailhead with rating 227 (because there are 121
distinct hiking trails that lead to the 9 on the right edge and 106 that lead to
the 9 on the bottom edge):

012345
123456
234567
345678
4.6789
56789.
Here's the larger example from before:

89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
Considering its trailheads in reading order, they have ratings of 20, 24, 10, 4,
1, 4, 5, 8, and 5. The sum of all trailhead ratings in this larger example
topographic map is 81.

You're not sure how, but the reindeer seems to have crafted some tiny flags out
of toothpicks and bits of paper and is using them to mark trailheads on your
topographic map. What is the sum of the ratings of all trailheads?

 */
#include <cstdlib>
#include <print>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "utils/utils.h"

using ::aoc::Coordinate;
using Trail = std::pair<Coordinate, Coordinate>;

class Map {
 public:
  static constexpr char kEmpty = '.';
  static constexpr char kAntinode = '#';
  explicit Map(std::vector<std::string> map) : map_(std::move(map)) {}

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
  int GetInt(Coordinate c) const { return ::aoc::ConvertCharToInt(GetChar(c)); }
  bool IsEmpty(Coordinate c) const {
    return GetChar(c) == kEmpty || GetChar(c) == kAntinode;
  }
  int NumRows() const { return map_.size(); }
  int NumCols() const { return map_.front().size(); }

 private:
  std::vector<std::string> map_;
};

// Returns the number of trailheads
int NumTrails(const Map& map, absl::flat_hash_set<Trail>& trails,
              Coordinate start, Coordinate c, int height = 0) {
  static constexpr int kMaxHeight = 9;
  if (map.IsOutOfBounds(c)) {
    // std::print("OOB\n");
    return 0;  // OOB
  }
  if (map.GetInt(c) != height) {
    // std::print("Invalid height...\n");
    return 0;  // Not valid height.
  }
  // std::print("Checking coordinate {} height {}\n", c.ToString(), height);
  if (height == kMaxHeight) {
    // std::print("Summit Reached!\n");
    trails.insert({start, c});
    return 1;  // Base case - we reached summit.
  }
  // Otherwise, recurse in all directions with the next height.
  ++height;
  return NumTrails(map, trails, start, GoRight(c), height) +
         NumTrails(map, trails, start, GoLeft(c), height) +
         NumTrails(map, trails, start, GoDown(c), height) +
         NumTrails(map, trails, start, GoUp(c), height);
}
int main() {
  Map map(aoc::LoadStringsFromFileByLine("./2024/day10.txt"));
  int num_paths = 0;
  // Trails are a start and end position that are connected.
  absl::flat_hash_set<Trail> trails;
  for (int row = 0; row < map.NumRows(); ++row) {
    for (int col = 0; col < map.NumCols(); ++col) {
      Coordinate position{row, col};
      num_paths += NumTrails(map, trails, position, position);
    }
  }

  // This info isn't even needed.
  std::print("Number of trails: {}\n", trails.size());
  std::print("Number of paths: {}\n", num_paths);

  return 0;
}
