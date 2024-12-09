/*
--- Day 14: Reindeer Olympics ---
This year is the Reindeer Olympics! Reindeer can fly at high speeds, but must
rest occasionally to recover their energy. Santa would like to know which of his
reindeer is fastest, and so he has them race.

Reindeer can only either be flying (always at their top speed) or resting (not
moving at all), and always spend whole seconds in either state.

For example, suppose you have the following Reindeer:

Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds.
Dancer can fly 16 km/s for 11 seconds, but then must rest for 162 seconds.
After one second, Comet has gone 14 km, while Dancer has gone 16 km. After ten
seconds, Comet has gone 140 km, while Dancer has gone 160 km. On the eleventh
second, Comet begins resting (staying at 140 km), and Dancer continues on for a
total distance of 176 km. On the 12th second, both reindeer are resting. They
continue to rest until the 138th second, when Comet flies for another ten
seconds. On the 174th second, Dancer flies for another 11 seconds.

In this example, after the 1000th second, both reindeer are resting, and Comet
is in the lead at 1120 km (poor Dancer has only gotten 1056 km by that point).
So, in this situation, Comet would win (if the race ended at 1000 seconds).

Given the descriptions of each reindeer (in your puzzle input), after exactly
2503 seconds, what distance has the winning reindeer traveled?
*/
#include <print>

#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include "utils/utils.h"

struct Reindeer {
  std::string name;
  int rate_km_per_s = 0;
  int run_duration_s = 0;
  int rest_duration_s = 0;
  int point_total = 0;
  int current_dist = 0;

  void AwardPoint() { ++point_total; }

  int CycleDuration() const { return run_duration_s + rest_duration_s; }
  int CycleDistance() const { return rate_km_per_s * run_duration_s; }
};

std::vector<Reindeer> GetReindeer(std::vector<std::string> lines) {
  constexpr int kNameIdx = 1;
  constexpr int kRateIdx = 2;
  constexpr int kRunDurationIdx = 3;
  constexpr int kRestDurationIdx = 4;

  // All the names we have seen so far.
  std::vector<Reindeer> reindeer;

  std::regex pattern(
      R"((\w+) can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.)");
  for (const auto& line : lines) {
    std::smatch match;
    std::regex_search(line, match, pattern);

    std::cout << match[kNameIdx] << " " << match[kRateIdx] << " "
              << match[kRunDurationIdx] << " " << match[kRestDurationIdx]
              << std::endl;
    reindeer.push_back(
        Reindeer{.name = match[kNameIdx],
                 .rate_km_per_s = std::stoi(match[kRateIdx]),
                 .run_duration_s = std::stoi(match[kRunDurationIdx]),
                 .rest_duration_s = std::stoi(match[kRestDurationIdx])});
  }
  return reindeer;
}

int ComputeDistAfterTime(const Reindeer& r, int time_s) {
  int dist = 0;
  // First, compute how many full cycles the reindeer ran.
  int num_full_cycles = time_s / r.CycleDuration();
  dist += r.CycleDistance() * num_full_cycles;

  // Subtract off the full cycles. Only a partial cycle remains.
  time_s %= r.CycleDuration();

  // Comput how many more seconds this reindeer witll run.
  int remaining_run_seconds = std::min(time_s, r.run_duration_s);

  dist += r.rate_km_per_s * remaining_run_seconds;

  return dist;
}
int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day14.txt");
  std::vector<Reindeer> reindeer = GetReindeer(lines);
  constexpr int kRaceDuration = 2503;
  // constexpr int kRaceDuration = 10;
  for (int i = 1; i <= kRaceDuration; ++i) {
    int max_dist = 0;
    for (auto& r : reindeer) {
      r.current_dist = ComputeDistAfterTime(r, i);
      max_dist = std::max(r.current_dist, max_dist);
      std::print("At {}, {} is at distance {}!\n", i, r.name, r.current_dist);
      // std::cout << "Time: "
    }
    std::for_each(reindeer.begin(), reindeer.end(), [max_dist](Reindeer& r) {
      if (r.current_dist == max_dist) {
        r.AwardPoint();
        std::print("Point awarded to {}! New total: {}.\n", r.name,
                   r.point_total);
      }
    });
  }
  for (auto& r : reindeer) {
    std::cout << r.name << " points: " << r.point_total << std::endl;
  }
  return 0;
}
