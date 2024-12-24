/*
--- Day 12: Garden Groups ---
Why not search for the Chief Historian near the gardener and his massive farm?
There's plenty of food, so The Historians grab something to eat while they
search.

You're about to settle near a complex arrangement of garden plots when some
Elves ask if you can lend a hand. They'd like to set up fences around each
region of garden plots, but they can't figure out how much fence they need to
order or how much it will cost. They hand you a map (your puzzle input) of the
garden plots.

Each garden plot grows only a single type of plant and is indicated by a single
letter on your map. When multiple garden plots are growing the same type of
plant and are touching (horizontally or vertically), they form a region. For
example:

AAAA
BBCD
BBCC
EEEC
This 4x4 arrangement includes garden plots growing five different types of
plants (labeled A, B, C, D, and E), each grouped into their own region.

In order to accurately calculate the cost of the fence around a single region,
you need to know that region's area and perimeter.

The area of a region is simply the number of garden plots the region contains.
The above map's type A, B, and C plants are each in a region of area 4. The type
E plants are in a region of area 3; the type D plants are in a region of area 1.

Each garden plot is a square and so has four sides. The perimeter of a region is
the number of sides of garden plots in the region that do not touch another
garden plot in the same region. The type A and C plants are each in a region
with perimeter 10. The type B and E plants are each in a region with
perimeter 8. The lone D plot forms its own region with perimeter 4.

Visually indicating the sides of plots in each region that contribute to the
perimeter using - and |, the above map's regions' perimeters are measured as
follows:

+-+-+-+-+
|A A A A|
+-+-+-+-+     +-+
              |D|
+-+-+   +-+   +-+
|B B|   |C|
+   +   + +-+
|B B|   |C C|
+-+-+   +-+ +
          |C|
+-+-+-+   +-+
|E E E|
+-+-+-+
Plants of the same type can appear in multiple separate regions, and regions can
even appear within other regions. For example:

OOOOO
OXOXO
OOOOO
OXOXO
OOOOO
The above map contains five regions, one containing all of the O garden plots,
and the other four each containing a single X plot.

The four X regions each have area 1 and perimeter 4. The region containing 21
type O plants is more complicated; in addition to its outer edge contributing a
perimeter of 20, its boundary with each X region contributes an additional 4 to
its perimeter, for a total perimeter of 36.

Due to "modern" business practices, the price of fence required for a region is
found by multiplying that region's area by its perimeter. The total price of
fencing all regions on a map is found by adding together the price of fence for
every region on the map.

In the first example, region A has price 4 * 10 = 40, region B has price 4 * 8 =
32, region C has price 4 * 10 = 40, region D has price 1 * 4 = 4, and region E
has price 3 * 8 = 24. So, the total price for the first example is 140.

In the second example, the region with all of the O plants has price 21 * 36 =
756, and each of the four smaller X regions has price 1 * 4 = 4, for a total
price of 772 (756 + 4 + 4 + 4 + 4).

Here's a larger example:

RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE
It contains:

A region of R plants with price 12 * 18 = 216.
A region of I plants with price 4 * 8 = 32.
A region of C plants with price 14 * 28 = 392.
A region of F plants with price 10 * 18 = 180.
A region of V plants with price 13 * 20 = 260.
A region of J plants with price 11 * 20 = 220.
A region of C plants with price 1 * 4 = 4.
A region of E plants with price 13 * 18 = 234.
A region of I plants with price 14 * 22 = 308.
A region of M plants with price 5 * 12 = 60.
A region of S plants with price 3 * 8 = 24.
So, it has a total price of 1930.

What is the total price of fencing all regions on your map?
 */
#include <cstdlib>
#include <print>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "utils/utils.h"

using ::aoc::Coordinate;
using ::aoc::Direction;
using ::aoc::GoDown;
using ::aoc::GoLeft;
using ::aoc::GoRight;
using ::aoc::GoUp;
using ::aoc::Map;
using ::aoc::VisitedMap;

struct Region {
  int64_t area{0};
  int64_t perimeter{0};
  int64_t sides{0};

  Region operator+(const Region& other) const {
    return {area + other.area, perimeter + other.perimeter};
  };
};

// Implement the algorithm described in the comment below.
int64_t CountCorner(const Map& map, VisitedMap& visited, char character,
                    Coordinate side_a, Coordinate side_b, Coordinate diagonal) {
  bool a_in_region = map.CharEquals(side_a, character);
  bool b_in_region = map.CharEquals(side_b, character);
  bool a_visited = visited.IsVisited(side_a);
  bool b_visited = visited.IsVisited(side_b);
  if ((a_in_region && a_visited) || (b_in_region && b_visited)) {
    // Don't double count if we've counted one of the sides already.
    return 0;
  }

  bool c_in_region = map.CharEquals(diagonal, character);

  if (!a_in_region && !b_in_region) {
    // If neither sides match, this is a corner.
    return 1;
  } else if ((a_in_region && !b_in_region) || (b_in_region && !a_in_region)) {
    if (c_in_region) {
      // Only one side matches, and so does the diagonal.
      return 1;
    }
  } else if (a_in_region && b_in_region) {
    if (!c_in_region) {
      // Both sides match, but the diagonal doesn't - this is also a corner.
      return 1;
    }
  }
  return 0;
}

int64_t CountCorners(const Map& map, VisitedMap& visited, Coordinate c) {
  // Types of corners:
  // There are three things worth checking.
  // These two:
  //
  // OOOOO
  // OOXOO
  // OOAXO
  //
  // If neither of these are 'A', this is a corner.
  // If one is an 'A', this might be a corner - check the diagonal:
  //   - If it is an 'A', this IS a corner.
  // If two of them are 'A', this is not a corner.
  // Only count them if these are _not_ visited.
  int64_t count = 0;
  char character = map.GetChar(c);
  count += CountCorner(map, visited, character, GoUp(c), GoRight(c),
                       GoUp(GoRight(c)));
  count += CountCorner(map, visited, character, GoRight(c), GoDown(c),
                       GoRight(GoDown(c)));
  count += CountCorner(map, visited, character, GoDown(c), GoLeft(c),
                       GoDown(GoLeft(c)));
  count +=
      CountCorner(map, visited, character, GoLeft(c), GoUp(c), GoLeft(GoUp(c)));

  std::print("Counted {} corners at coordinate {}.\n", count, c.ToString());

  return count;
}

void ComputeRegion(const Map& map, VisitedMap& visited, Region& region,
                   char region_char, Coordinate current) {
  // OOB - this is a perimeter edge.
  if (map.IsOutOfBounds(current)) {
    region.perimeter++;
    return;
  }
  // Wrong char. This is a perimeter edge.
  if (map.GetChar(current) != region_char) {
    region.perimeter++;
    return;
  }
  // visited. Do nothing.
  if (visited.IsVisited(current)) {
    return;
  }
  // Otherwise, this is a part of the region.
  // Mark it as visited and include it in the area, and recurse.
  region.area++;
  visited.MarkVisited(current);

  region.sides += CountCorners(map, visited, current);

  // std::print("Counted {} corners so far at {}\n", region.sides,
  //            current.ToString());

  ComputeRegion(map, visited, region, region_char, GoUp(current));
  ComputeRegion(map, visited, region, region_char, GoRight(current));
  ComputeRegion(map, visited, region, region_char, GoLeft(current));
  ComputeRegion(map, visited, region, region_char, GoDown(current));

  return;
}

struct Price {
  int64_t price{0};
  int64_t bulk{0};

  Price operator+(const Price& other) const {
    return {price + other.price, bulk + other.bulk};
  };

  // Addition assignment operator
  Price& operator+=(const Price& other) {
    price += other.price;
    bulk += other.bulk;
    return *this;
  }
};

Price RegionPrice(const Map& map, VisitedMap& visited, Coordinate start) {
  // If visited, return 0.
  if (visited.IsVisited(start)) {
    return {};
  }

  // This is an uncomputed region. Compute it.
  char region_char = map.GetChar(start);
  Region region;

  ComputeRegion(map, visited, region, region_char, start);

  std::print("Region {} starting at {} is {} area, {} perimeter, {} sides.\n",
             region_char, start.ToString(), region.area, region.perimeter,
             region.sides);

  return {.price = region.area * region.perimeter,
          .bulk = region.area * region.sides};
}

int main() {
  // Map map(aoc::LoadStringsFromFileByLine("./2024/day12example.txt"));
  Map map(aoc::LoadStringsFromFileByLine("./2024/day12.txt"));

  VisitedMap visited(map.NumRows(), map.NumCols());

  Price price{};
  // Trails are a start and end position that are connected.
  for (int row = 0; row < map.NumRows(); ++row) {
    for (int col = 0; col < map.NumCols(); ++col) {
      Coordinate position{row, col};
      price += RegionPrice(map, visited, position);
    }
  }

  // This info isn't even needed.
  std::print("Price: {}\nBulk Price: {}\n", price.price, price.bulk);

  return 0;
}
