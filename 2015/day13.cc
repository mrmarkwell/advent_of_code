/*
--- Day 13: Knights of the Dinner Table ---
In years past, the holiday feast with your family hasn't gone so well. Not
everyone gets along! This year, you resolve, will be different. You're going to
find the optimal seating arrangement and avoid all those awkward conversations.

You start by writing up a list of everyone invited and the amount their
happiness would increase or decrease if they were to find themselves sitting
next to each other person. You have a circular table that will be just big
enough to fit everyone comfortably, and so each person will have exactly two
neighbors.

For example, suppose you have only four attendees planned, and you calculate
their potential happiness as follows:

Alice would gain 54 happiness units by sitting next to Bob.
Alice would lose 79 happiness units by sitting next to Carol.
Alice would lose 2 happiness units by sitting next to David.
Bob would gain 83 happiness units by sitting next to Alice.
Bob would lose 7 happiness units by sitting next to Carol.
Bob would lose 63 happiness units by sitting next to David.
Carol would lose 62 happiness units by sitting next to Alice.
Carol would gain 60 happiness units by sitting next to Bob.
Carol would gain 55 happiness units by sitting next to David.
David would gain 46 happiness units by sitting next to Alice.
David would lose 7 happiness units by sitting next to Bob.
David would gain 41 happiness units by sitting next to Carol.
Then, if you seat Alice next to David, Alice would lose 2 happiness units
(because David talks so much), but David would gain 46 happiness units (because
Alice is such a good listener), for a total change of 44.

If you continue around the table, you could then seat Bob next to Alice (Bob
gains 83, Alice gains 54). Finally, seat Carol, who sits next to Bob (Carol
gains 60, Bob loses 7) and David (Carol gains 55, David gains 41). The
arrangement looks like this:

     +41 +46
+55   David    -2
Carol       Alice
+60    Bob    +54
     -7  +83
After trying every other seating arrangement in this hypothetical scenario, you
find that this one is the most optimal, with a total change in happiness of 330.

What is the total change in happiness for the optimal seating arrangement of the
actual guest list?
*/

#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/utils.h"

int NameToNode(const std::string& name, std::vector<std::string>& names) {
  // If we have seen the name, return its idx.
  auto it = std::find(names.begin(), names.end(), name);
  if (it != names.end()) {
    // Return the index.
    return std::distance(names.begin(), it);
  }
  names.push_back(name);
  return names.size() - 1;
}

void AddValueToMatrix(std::vector<std::vector<int>>& matrix, int from_node,
                      int to_node, int value) {
  int required_size = std::max(from_node, to_node) + 1;
  if (matrix.size() < required_size) {
    matrix.resize(required_size);
    for (auto& vec : matrix) {
      vec.resize(required_size);
    }
  }
  matrix[from_node][to_node] = value;
}

std::vector<std::vector<int>> BuildMatrix(std::vector<std::string> lines) {
  constexpr int kFromIdx = 1;
  constexpr int kToIdx = 4;
  constexpr int kSignIdx = 2;
  constexpr int kValueIdx = 3;

  std::vector<std::vector<int>> matrix;

  // All the names we have seen so far.
  std::vector<std::string> names;

  std::regex number_pattern(
      R"((\w+) would (\w+) (\d+) happiness units by sitting next to (\w+).)");
  for (const auto& line : lines) {
    std::smatch match;
    std::regex_search(line, match, number_pattern);

    int from_node = NameToNode(match[kFromIdx], names);
    int to_node = NameToNode(match[kToIdx], names);

    AddValueToMatrix(matrix, from_node, to_node, std::stoi(match[kValueIdx]));
    if (match[kSignIdx] == "lose") {
      matrix[from_node][to_node] *= -1;
    }
  }
  return matrix;
}

// Function to pretty print a 2D vector
void PrettyPrint(const std::vector<std::vector<int>>& matrix) {
  for (const auto& row : matrix) {
    std::cout << "[ ";
    for (const auto& elem : row) {
      std::cout << elem << " ";
    }
    std::cout << "]" << std::endl;
  }
}

void PrettyPrint(const std::vector<int>& vec, int happiness) {
  std::cout << "[ ";
  for (const auto& elem : vec) {
    std::cout << elem << " ";
  }
  std::cout << "] -> " << happiness << std::endl;
}

int ComputeHappiness(const std::vector<int>& table,
                     const std::vector<std::vector<int>>& matrix,
                     bool print = false) {
  int happiness = 0;
  for (int person_idx = 0; person_idx < table.size(); ++person_idx) {
    // For each person, add the happiness from their left & right table partner.
    int left_idx = person_idx - 1;
    int right_idx = person_idx + 1;
    // handle rollover.
    if (left_idx < 0) {
      left_idx += table.size();
    }
    right_idx %= table.size();
    int left_neighbor = table[left_idx];
    int right_neighbor = table[right_idx];
    int person = table[person_idx];
    happiness += matrix[person][left_neighbor];
    happiness += matrix[person][right_neighbor];
    if (print)
      std::cout << "Person: " << person
                << " Left: " << matrix[person][left_neighbor]
                << " Right: " << matrix[person][right_neighbor] << "\n";
  }
  // PrettyPrint(table, happiness);
  return happiness;
}

bool AlreadyAtTable(const std::vector<int>& table,

                    int current_position, int person) {
  auto end_iter = table.begin() + current_position;
  auto it = std::find(table.begin(), end_iter, person);
  // Return true if we _did_ find the person.
  return it != end_iter;
}

void FindMaxHappiness(const std::vector<std::vector<int>>& matrix,
                      std::vector<int>& table, int current_position,
                      int& happiness) {
  if (current_position == table.size()) {
    int old_happiness = happiness;
    happiness = std::max(happiness, ComputeHappiness(table, matrix));

    // Done with this branch.
    return;
  }

  for (int person = 1; person < table.size(); ++person) {
    if (AlreadyAtTable(table, current_position, person)) {
      continue;
    }
    // We haven't seated this sir or lady. Place them here and recurse.
    table[current_position] = person;
    FindMaxHappiness(matrix, table, current_position + 1, happiness);
  }
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day13.txt");

  std::vector<std::vector<int>> matrix = BuildMatrix(lines);

  // PrettyPrint(matrix);

  std::vector<int> table(matrix.size());

  // Force person 0 to position 0.
  table[0] = 0;

  int happiness = 0;

  // Starting position is 1, since we force person 0 to seat 0.
  constexpr int kStartingPosition = 1;
  FindMaxHappiness(matrix, table, kStartingPosition, happiness);

  // Update the matrix to include the neutral person.
  // Add a row of zeroes.
  matrix.push_back(std::vector<int>(matrix.front().size(), 0));
  // add a column of zeroes.
  std::for_each(matrix.begin(), matrix.end(),
                [](auto& row) { row.push_back(0); });
  // Add a seat at the table.
  table.push_back(0);
  int new_happiness = 0;
  FindMaxHappiness(matrix, table, kStartingPosition, new_happiness);

  std::cout << "Max happiness: " << happiness << std::endl;

  // PrettyPrint(matrix);

  std::cout << "New max happiness: " << new_happiness << std::endl;

  return 0;
}
