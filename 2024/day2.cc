/*
--- Day 2: Red-Nosed Reports ---
Fortunately, the first location The Historians want to search isn't a long walk
from the Chief Historian's office.

While the Red-Nosed Reindeer nuclear fusion/fission plant appears to contain no
sign of the Chief Historian, the engineers there run up to you as soon as they
see you. Apparently, they still talk about the time Rudolph was saved through
molecular synthesis from a single electron.

They're quick to add that - since you're already here - they'd really appreciate
your help analyzing some unusual data from the Red-Nosed reactor. You turn to
check if The Historians are waiting for you, but they seem to have already
divided into groups that are currently searching every corner of the facility.
You offer to help with the unusual data.

The unusual data (your puzzle input) consists of many reports, one report per
line. Each report is a list of numbers called levels that are separated by
spaces. For example:

7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
This example data contains six reports each containing five levels.

The engineers are trying to figure out which reports are safe. The Red-Nosed
reactor safety systems can only tolerate levels that are either gradually
increasing or gradually decreasing. So, a report only counts as safe if both of
the following are true:

The levels are either all increasing or all decreasing.
Any two adjacent levels differ by at least one and at most three.
In the example above, the reports can be found safe or unsafe by checking those
rules:

7 6 4 2 1: Safe because the levels are all decreasing by 1 or 2.
1 2 7 8 9: Unsafe because 2 7 is an increase of 5.
9 7 6 2 1: Unsafe because 6 2 is a decrease of 4.
1 3 2 4 5: Unsafe because 1 3 is increasing but 3 2 is decreasing.
8 6 4 4 1: Unsafe because 4 4 is neither an increase or a decrease.
1 3 6 7 9: Safe because the levels are all increasing by 1, 2, or 3.
So, in this example, 2 reports are safe.

Analyze the unusual data from the engineers. How many reports are safe?

--- Part Two ---
The engineers are surprised by the low number of safe reports until they realize
they forgot to tell you about the Problem Dampener.

The Problem Dampener is a reactor-mounted module that lets the reactor safety
systems tolerate a single bad level in what would otherwise be a safe report.
It's like the bad level never happened!

Now, the same rules apply as before, except if removing a single level from an
unsafe report would make it safe, the report instead counts as safe.

More of the above example's reports are now safe:

7 6 4 2 1: Safe without removing any level.
1 2 7 8 9: Unsafe regardless of which level is removed.
9 7 6 2 1: Unsafe regardless of which level is removed.
1 3 2 4 5: Safe by removing the second level, 3.
8 6 4 4 1: Safe by removing the third level, 4.
1 3 6 7 9: Safe without removing any level.
Thanks to the Problem Dampener, 4 reports are actually safe!

Update your analysis by handling situations where the Problem Dampener can
remove a single level from unsafe reports. How many reports are now safe?
*/

#include <cstdlib>

#include "absl/log/check.h"
#include "utils/utils.h"

bool IsSafe(const std::vector<int64_t>& report) {
  bool should_increase = report[0] < report[1];
  bool should_decrease = !should_increase;
  for (int i = 1; i < report.size(); ++i) {
    int64_t prev = report[i - 1];
    int64_t next = report[i];
    if (should_increase && prev >= next) {
      // Not increasing!
      return false;
    }
    if (should_decrease && prev <= next) {
      // Increasing but should be decreasing!
      return false;
    }
    if (std::abs(next - prev) > 3) {
      return false;
    }
  }
  return true;
}

std::vector<int64_t> RemoveIndex(const std::vector<int64_t>& input,
                                 size_t index) {
  // Use CHECK to ensure the index is within bounds.
  CHECK(index < input.size())
      << "Index out of range: " << index << ", size: " << input.size();

  std::vector<int64_t> result;
  result.reserve(input.size() - 1);  // Reserve space for efficiency.

  // Copy elements, skipping the specified index.
  for (size_t i = 0; i < input.size(); ++i) {
    if (i != index) {
      result.push_back(input[i]);
    }
  }

  return result;
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("2024/day2.txt");

  std::vector<std::vector<int64_t>> reports;
  for (const auto& line : lines) {
    std::vector<int64_t> values;
    std::vector<std::string> number_str = aoc::SplitDelimitedString(line);
    for (const auto& number : aoc::SplitDelimitedString(line)) {
      absl::StatusOr<int64_t> value = aoc::ConvertStringViewToInt64(number);
      CHECK_OK(value);
      values.push_back(*value);
    }
    reports.push_back(std::move(values));
  }

  int num_safe_reports = 0;
  int num_safe_reports_part_2 = 0;
  for (const auto& report : reports) {
    if (IsSafe(report)) {
      ++num_safe_reports;
      ++num_safe_reports_part_2;
    } else {
      // Try finding a permutation that is safe.
      for (int i = 0; i < report.size(); ++i) {
        if (IsSafe(RemoveIndex(report, i))) {
          ++num_safe_reports_part_2;
          break;
        }
      }
    }
  }

  // std::print("Number of safe reports: {}\n", num_safe_reports);
  // std::print("Number of safe reports (part 2): {}\n",
  // num_safe_reports_part_2);

  return 0;
}
