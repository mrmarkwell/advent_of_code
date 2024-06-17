/*
--- Day 9: All in a Single Night ---
Every year, Santa manages to deliver all of his presents in a single night.

This year, however, he has some new locations to visit; his elves have provided
him the distances between every pair of locations. He can start and end at any
two (different) locations he wants, but he must visit each location exactly
once. What is the shortest distance he can travel to achieve this?

For example, given the following distances:

London to Dublin = 464
London to Belfast = 518
Dublin to Belfast = 141
The possible routes are therefore:

Dublin -> London -> Belfast = 982
London -> Dublin -> Belfast = 605
London -> Belfast -> Dublin = 659
Dublin -> Belfast -> London = 659
Belfast -> Dublin -> London = 605
Belfast -> London -> Dublin = 982
The shortest of these is London -> Dublin -> Belfast = 605, and so the answer is
605 in this example.

What is the distance of the shortest route?

--- Part Two ---
The next year, just to show off, Santa decides to take the route with the
longest distance instead.

He can still start and end at any two (different) locations he wants, and he
still must visit each location exactly once.

For example, given the distances above, the longest route would be 982 via (for
example) Dublin -> London -> Belfast.

What is the distance of the longest route?
*/

#include <climits>
#include <exception>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/utils.h"

std::vector<std::vector<int>> BuildMatrix(
    std::vector<std::string> lines,
    const std::unordered_map<std::string, int>& map) {
  int length = map.size();
  std::vector<std::vector<int>> matrix(length, std::vector<int>(length));

  for (const auto& line : lines) {
    std::regex number_pattern(R"((\w+) to (\w+) = (\d+))");
    std::smatch match;
    std::regex_search(line, match, number_pattern);

    std::string from_name = match[1];
    std::string to_name = match[2];

    int from_int = map.at(from_name);
    int to_int = map.at(to_name);
    matrix[from_int][to_int] = std::stoi(match[3]);
    matrix[to_int][from_int] = std::stoi(match[3]);
  }
  return matrix;
}

std::unordered_map<std::string, int> BuildStringMap(
    std::vector<std::string> lines) {
  int id = 0;
  std::unordered_map<std::string, int> map;
  for (const auto& line : lines) {
    std::regex number_pattern(R"((\w+) to (\w+) = (\d+))");
    std::smatch match;
    std::regex_search(line, match, number_pattern);

    std::string from_name = match[1];
    std::string to_name = match[2];

    if (map.find(from_name) == map.end()) {
      map.insert({from_name, id});
      id++;
    }
    if (map.find(to_name) == map.end()) {
      map.insert({to_name, id});
      id++;
    }
  }
  return map;
}

int MinPath(const std::vector<std::vector<int>>& graph, int s) {
  int V = graph.size();  // calculate number of vertices

  // store all vertices apart from the source vertex
  std::vector<int> vertex;
  for (int i = 0; i < V; i++)
    if (i != s) vertex.push_back(i);

  // store minimum weight Hamiltonian Path (no need to return to source)
  int min_path = INT_MAX;

  do {
    // store current path weight (cost)
    int current_pathweight = 0;

    // compute current path weight
    int k = s;
    for (int i = 0; i < vertex.size(); i++) {
      current_pathweight += graph[k][vertex[i]];
      k = vertex[i];
    }

    // Update the minimum path cost
    min_path = std::min(min_path, current_pathweight);

  } while (std::next_permutation(vertex.begin(), vertex.end()));

  return min_path;
}

int MaxPath(const std::vector<std::vector<int>>& graph, int s) {
  int V = graph.size();  // calculate number of vertices

  // store all vertices apart from the source vertex
  std::vector<int> vertex;
  for (int i = 0; i < V; i++)
    if (i != s) vertex.push_back(i);

  // store minimum weight Hamiltonian Path (no need to return to source)
  int max_path = INT_MIN;

  do {
    // store current path weight (cost)
    int current_pathweight = 0;

    // compute current path weight
    int k = s;
    for (int i = 0; i < vertex.size(); i++) {
      current_pathweight += graph[k][vertex[i]];
      k = vertex[i];
    }

    // Update the maximum path cost
    max_path = std::max(max_path, current_pathweight);

  } while (std::next_permutation(vertex.begin(), vertex.end()));

  return max_path;
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day9.txt");

  std::unordered_map<std::string, int> string_map = BuildStringMap(lines);
  std::vector<std::vector<int>> adjacency_matrix =
      BuildMatrix(lines, string_map);

  int min_path = INT_MAX;
  for (int i = 0; i < adjacency_matrix.size(); i++) {
    int temp = MinPath(adjacency_matrix, i);
    if (temp < min_path) {
      min_path = temp;
    }
  }

  std::cout << "The minimum path is " << min_path << std::endl;

  int max_path = INT_MIN;
  for (int i = 0; i < adjacency_matrix.size(); i++) {
    int temp = MaxPath(adjacency_matrix, i);
    if (temp > max_path) {
      max_path = temp;
    }
  }

  std::cout << "The maximum path is " << max_path << std::endl;

  return 0;
}