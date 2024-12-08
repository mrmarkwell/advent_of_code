/*
The Historians use their fancy device again, this time to whisk you all away to
the North Pole prototype suit manufacturing lab... in the year 1518! It turns
out that having direct access to history is very convenient for a group of
historians.

You still have to be careful of time paradoxes, and so it will be important to
avoid anyone from 1518 while The Historians search for the Chief. Unfortunately,
a single guard is patrolling this part of the lab.

Maybe you can work out where the guard will go ahead of time so that The
Historians can search safely?

You start by making a map (your puzzle input) of the situation. For example:

....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
The map shows the current position of the guard with ^ (to indicate the guard is
currently facing up from the perspective of the map). Any obstructions - crates,
desks, alchemical reactors, etc. - are shown as #.

Lab guards in 1518 follow a very strict patrol protocol which involves
repeatedly following these steps:

If there is something directly in front of you, turn right 90 degrees.
Otherwise, take a step forward.
Following the above protocol, the guard moves up several times until she reaches
an obstacle (in this case, a pile of failed suit prototypes):

....#.....
....^....#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...
Because there is now an obstacle in front of the guard, she turns right before
continuing straight in her new facing direction:

....#.....
........>#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#...
Reaching another obstacle (a spool of several very long polymers), she turns
right again and continues downward:

....#.....
.........#
..........
..#.......
.......#..
..........
.#......v.
........#.
#.........
......#...
This process continues for a while, but the guard eventually leaves the mapped
area (after walking past a tank of universal solvent):

....#.....
.........#
..........
..#.......
.......#..
..........
.#........
........#.
#.........
......#v..
By predicting the guard's route, you can determine which specific positions in
the lab will be in the patrol path. Including the guard's starting position, the
positions visited by the guard before leaving the area are marked with an X:

....#.....
....XXXXX#
....X...X.
..#.X...X.
..XXXXX#X.
..X.X.X.X.
.#XXXXXXX.
.XXXXXXX#.
#XXXXXXX..
......#X..
In this example, the guard will visit 41 distinct positions on your map.

Predict the path of the guard. How many distinct positions will the guard visit
before leaving the mapped area?

--- Part Two ---
While The Historians begin working around the guard's patrol route, you borrow
their fancy device and step outside the lab. From the safety of a supply closet,
you time travel through the last few months and record the nightly status of the
lab's guard post on the walls of the closet.

Returning after what seems like only a few seconds to The Historians, they
explain that the guard's patrol area is simply too large for them to safely
search the lab without getting caught.

Fortunately, they are pretty sure that adding a single new obstruction won't
cause a time paradox. They'd like to place the new obstruction in such a way
that the guard will get stuck in a loop, making the rest of the lab safe to
search.

To have the lowest chance of creating a time paradox, The Historians would like
to know all of the possible positions for such an obstruction. The new
obstruction can't be placed at the guard's starting position - the guard is
there right now and would notice.

In the above example, there are only 6 different positions where a new
obstruction would cause the guard to get stuck in a loop. The diagrams of these
six situations use O to mark the new obstruction, | to show a position where the
guard moves up/down, - to show a position where the guard moves left/right, and
+ to show a position where the guard moves both up/down and left/right.

Option one, put a printing press next to the guard's starting position:

....#.....
....+---+#
....|...|.
..#.|...|.
....|..#|.
....|...|.
.#.O^---+.
........#.
#.........
......#...
Option two, put a stack of failed suit prototypes in the bottom right quadrant
of the mapped area:


....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
......O.#.
#.........
......#...
Option three, put a crate of chimney-squeeze prototype fabric next to the
standing desk in the bottom right quadrant:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----+O#.
#+----+...
......#...
Option four, put an alchemical retroencabulator near the bottom left corner:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
..|...|.#.
#O+---+...
......#...
Option five, put the alchemical retroencabulator a bit to the right instead:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
....|.|.#.
#..O+-+...
......#...
Option six, put a tank of sovereign glue right next to the tank of universal
solvent:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----++#.
#+----++..
......#O..
It doesn't really matter what you choose to use as an obstacle so long as you
and The Historians can put it into position without the guard noticing. The
important thing is having enough options that you can find one that minimizes
time paradoxes, and in this example, there are 6 different positions you could
choose.

You need to get the guard stuck in a loop by adding a single new obstruction.
How many different positions could you choose for this obstruction?

*/
#include <cstdlib>

#include "absl/log/check.h"
#include "fmt/core.h"
#include "utils/utils.h"

struct Coordinate {
  int row{0};
  int col{0};
};

class Map {
 public:
  static constexpr char kVisited = 'X';
  static constexpr char kOutOfBounds = 'O';
  static constexpr char kObstacle = '#';
  static constexpr char kEmpty = '.';
  explicit Map(std::vector<std::string> map) : map_(std::move(map)) {
    for (int row = 0; row < map_.size(); ++row) {
      auto col = map_[row].find_first_of("^>v<");
      if (col != std::string::npos) {
        // We found the guard! Mark his position.
        guard_ = {row, static_cast<int>(col)};
        break;
      }
    }
    // This should never happen. The guard couldn't be found.
    CHECK(guard_.has_value());
  }

  // Returns true if the guard is present.
  bool GuardPresent() { return guard_.has_value(); }

  // Tick forward time by one step.
  void Tick() {
    if (!GuardPresent()) {
      // Nothing to do.
      return;
    }
    char& g = GetChar(*guard_);
    CHECK(g != kVisited) << "Shouldn't be possible???";

    Coordinate next = NextCoordinate();

    // fmt::print("Next row: {} col: {}\n", next.row, next.col);

    if (OutOfBounds(next)) {
      // fmt::print("OOB\n");
      //  Visit the current location and remove the guard from the map.
      g = kVisited;
      guard_ = std::nullopt;
      return;
    }
    char next_char = GetChar(next);
    if (next_char == kEmpty || next_char == kVisited) {
      // fmt::print("Empty\n");
      //  Move the guard coordinate to the next one, and move the guard.
      guard_ = next;
      GetChar(*guard_) = g;
      // Mark this spot as visited.
      g = kVisited;
    }
    if (next_char == kObstacle) {
      //  fmt::print("Obstacle\n");
      // Turn right only. We won't visit anything this time step.
      TurnRight();
    }
  }
  int CountVisited() {
    int visited = 0;
    for (std::string_view row : map_) {
      visited += std::count(row.begin(), row.end(), kVisited);
    }
    return visited;
  }

  void Print() {
    for (std::string_view row : map_) {
      fmt::print("{}\n", row);
    }
  }

 private:
  void TurnRight() {
    char& guard_char = GetChar(*guard_);
    switch (guard_char) {
      case '^':
        guard_char = '>';
        return;
      case '>':
        guard_char = 'v';
        return;
      case 'v':
        guard_char = '<';
        return;
      case '<':
        guard_char = '^';
        return;
    }
  }

  Coordinate NextCoordinate() {
    Coordinate next = *guard_;
    char guard_char = GetChar(next);
    if (guard_char == '^') next.row--;
    if (guard_char == '>') next.col++;
    if (guard_char == 'v') next.row++;
    if (guard_char == '<') next.col--;
    return next;
  }

  bool OutOfBounds(Coordinate c) {
    bool in_bounds = c.row >= 0 && c.row < map_.size() && c.col >= 0 &&
                     c.col < map_.front().size();
    return !in_bounds;
  }
  char& GetChar(Coordinate c) { return map_[c.row][c.col]; }
  std::vector<std::string> map_;
  std::optional<Coordinate> guard_;
};
int main() {
  Map map(aoc::LoadStringsFromFileByLine("./2024/day6.txt"));

  while (map.GuardPresent()) {
    // fmt::print("Step!\n");
    map.Tick();
    // map.Print();
  }
  map.Print();
  fmt::print("Total Visited: {}\n", map.CountVisited());
}
