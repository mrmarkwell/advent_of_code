#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cassert>

#include "utils/utils.h"

struct Neighbors {
  int on = 0;
  int off = 0;
};

void PrettyPrint(const std::vector<std::string>& vec) {
  fmt::print("[");
  for (size_t i = 0; i < vec.size(); ++i) {
    fmt::print("\"{}\"", vec[i]);
    if (i != vec.size() - 1) {
      fmt::print(", ");
    }
  }
  fmt::print("]\n");
}

bool IsInBounds(const std::vector<std::string>& lights, int row, int col) {
  if (row < 0 || row >= lights.size() || col < 0 || col >= lights[0].size()) {
    return false;
  }
  return true;
}

Neighbors GetNeighborStates(const std::vector<std::string>& lights, int row,
                            int col) {
  Neighbors neighbors;
  for (int i = row - 1; i <= row + 1; i++) {
    for (int j = col - 1; j <= col + 1; j++) {
      if (i == row && j == col) {
        continue;
      } else {
        if (IsInBounds(lights, i, j)) {
          if (lights[i][j] == '#') {
            neighbors.on++;
          } else {
            neighbors.off++;
          }
        }
      }
    }
  }
  return neighbors;
}

std::vector<std::string> GetNextState(const std::vector<std::string>& lights,
                                      bool corners_stuck = false) {
  std::vector<std::string> next_state(lights.size(),
                                      std::string(lights.size(), '.'));
  for (int i = 0; i < lights.size(); i++) {
    for (int j = 0; j < lights.size(); j++) {
      Neighbors neighbors = GetNeighborStates(lights, i, j);
      if (lights[i][j] == '#') {
        if (neighbors.on == 2 || neighbors.on == 3) {
          next_state[i][j] = '#';
        }
      } else {
        if (neighbors.on == 3) {
          next_state[i][j] = '#';
        }
      }
    }
  }
  if (corners_stuck) {
    next_state[0][0] = '#';
    next_state[0][next_state.size() - 1] = '#';
    next_state[next_state.size() - 1][0] = '#';
    next_state[next_state.size() - 1][next_state.size() - 1] = '#';
  }

  return next_state;
}

int CountOnLights(std::vector<std::string> lights) {
  int count = 0;
  for (int i = 0; i < lights.size(); i++) {
    for (int j = 0; j < lights.size(); j++) {
      if (lights[i][j] == '#') {
        count++;
      }
    }
  }
  return count;
}

void TestGetNeighborStates() {
  std::vector<std::string> lights = {
      {"#...#"}, {"##..."}, {"###.#"}, {"..#.."}, {".#..#"}};

  Neighbors neighbors = GetNeighborStates(lights, 0, 0);
  assert(neighbors.on == 2 && neighbors.off == 1);
  neighbors = GetNeighborStates(lights, 0, 2);
  assert(neighbors.on == 1 && neighbors.off == 4);
  neighbors = GetNeighborStates(lights, 2, 2);
  assert(neighbors.on == 3 && neighbors.off == 5);
}

void TestGetNextState() {
  std::vector<std::string> state1 = {{".#.#.#"}, {"...##."}, {"#....#"},
                                     {"..#..."}, {"#.#..#"}, {"####.."}};
  state1 = GetNextState(state1);
  std::vector<std::string> state2 = {
      {"..##.."}, {"..##.#"}, {"...##."}, {"......"}, {"#....."}, {"#.##.."},
  };
  // PrettyPrint(state1);
  // PrettyPrint(state2);
  assert(state1 == state2);
};

int main() {
  std::vector<std::string> initial_lights =
      aoc::LoadStringsFromFileByLine("./2015/day18.txt");

  TestGetNeighborStates();
  TestGetNextState();

  std::vector<std::string> new_lights = initial_lights;
  for (int i = 0; i < 100; i++) {
    new_lights = GetNextState(new_lights);
  }

  int lights_on = CountOnLights(new_lights);
  fmt::print("After 100 iterations, {} lights are on.\n", lights_on);

  new_lights = initial_lights;
  for (int i = 0; i < 100; i++) {
    new_lights = GetNextState(new_lights, true);
  }

  lights_on = CountOnLights(new_lights);
  fmt::print(
      "After 100 iterations with the corner lights stuck, {} lights are on.\n",
      lights_on);

  return 0;
}