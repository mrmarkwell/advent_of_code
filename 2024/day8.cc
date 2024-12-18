/*
 */
#include <cstdlib>
#include <print>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/log/check.h"
#include "utils/utils.h"

using ::aoc::Coordinate;

class Map {
 public:
  explicit Map(std::vector<std::string> map) : map_(std::move(map)) {}

  absl::flat_hash_set<Coordinate> ComputeAntinodes() {
    absl::flat_hash_set<Coordinate> antinodes;

    return antinodes;
  }

 private:
  std::vector<std::string> map_;
};

int main() {
  Map map{aoc::LoadStringsFromFileByLine("./2024/day8.txt")};

  // Keep a set of antinode coordinates.
  absl::flat_hash_set<Coordinate> antinodes = map.ComputeAntinodes();

  return 0;
}
