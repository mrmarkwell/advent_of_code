/*
--- Day 17: No Such Thing as Too Much ---
The elves bought too much eggnog again - 150 liters this time. To fit it all
into your refrigerator, you'll need to move it into smaller containers. You take
an inventory of the capacities of the available containers.

For example, suppose you have containers of size 20, 15, 10, 5, and 5 liters. If
you need to store 25 liters, there are four ways to do it:

15 and 10
20 and 5 (the first 5)
20 and 5 (the second 5)
15, 5, and 5
Filling all containers entirely, how many different combinations of containers
can exactly fit all 150 liters of eggnog?

Your puzzle answer was 1638.

The first half of this puzzle is complete! It provides one gold star: *

--- Part Two ---
While playing with all the containers in the kitchen, another load of eggnog
arrives! The shipping and receiving department is requesting as many containers
as you can spare.

Find the minimum number of containers that can exactly fit all 150 liters of
eggnog. How many different ways can you fill that number of containers and still
hold exactly 150 litres?

In the example above, the minimum number of containers was two. There were three
ways to use that many containers, and so the answer there would be 3.
*/

#include <cassert>
#include <print>

#include "utils/utils.h"

std::vector<int> LoadData() {
  std::vector<int> jugs;
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day17.txt");

  for (auto line : lines) {
    jugs.push_back(std::stoi(line));
  }
  return jugs;
}

void FindJugCombinations(std::vector<int> unused_jugs, int remaining_eggnog,
                         int used_jugs, int& min_used_jugs,
                         int& num_of_combinations, bool part2 = false) {
  if (remaining_eggnog < 0) {
    return;
  } else if (remaining_eggnog == 0) {
    if (!part2) {
      num_of_combinations++;
      return;
    }
    if (used_jugs == min_used_jugs) {
      num_of_combinations++;
      return;
    } else if (used_jugs < min_used_jugs) {
      min_used_jugs = used_jugs;
      num_of_combinations = 1;
      return;
    }
  }

  for (int i = 0; i < unused_jugs.size(); i++) {
    int jug = unused_jugs[i];
    std::vector<int> next_unused_jugs = unused_jugs;
    // Remove all jugs to the left since we don't want to repeat the check.
    next_unused_jugs.erase(next_unused_jugs.begin(),
                           next_unused_jugs.begin() + i + 1);
    // Recurse, using one more jug.
    FindJugCombinations(next_unused_jugs, remaining_eggnog - jug, used_jugs + 1,
                        min_used_jugs, num_of_combinations, part2);
  }
  return;
}

void TestPart1() {
  std::vector<int> test_values = {20, 15, 10, 5, 5};
  int num_of_combinations = 0;
  int min_jugs_used = test_values.size();
  int used_jugs = 0;
  FindJugCombinations(test_values, 25, used_jugs, min_jugs_used,
                      num_of_combinations);
  assert(num_of_combinations == 4);
  // std::print("Test1: {}\n", num_of_combinations);
}

void TestPart2() {
  std::vector<int> test_values = {20, 15, 10, 5, 5};
  int num_of_combinations = 0;
  int min_used_jugs = test_values.size();
  int used_jugs = 0;
  FindJugCombinations(test_values, 25, used_jugs, min_used_jugs,
                      num_of_combinations, true);
  // std::print("Test2: {}\n", num_of_combinations);
  assert(num_of_combinations == 3);
}

int main() {
  std::vector<int> jugs = LoadData();
  TestPart1();
  int combinations = 0;
  int min_used_jugs = jugs.size();
  constexpr int used_jugs = 0;
  FindJugCombinations(jugs, 150, used_jugs, min_used_jugs, combinations);

  std::print("There are {} valid combinations.\n", combinations);

  TestPart2();
  combinations = 0;
  min_used_jugs = jugs.size();
  FindJugCombinations(jugs, 150, used_jugs, min_used_jugs, combinations, true);

  std::print(
      "There are {} different ways to send {} jugs to shipping and "
      "recieving.\n",
      combinations, min_used_jugs);
  return 0;
}
