/*
--- Day 24: It Hangs in the Balance ---
It's Christmas Eve, and Santa is loading up the sleigh for this year's
deliveries. However, there's one small problem: he can't get the sleigh to
balance. If it isn't balanced, he can't defy physics, and nobody gets presents
this year.

No pressure.

Santa has provided you a list of the weights of every package he needs to fit on
the sleigh. The packages need to be split into three groups of exactly the same
weight, and every package has to fit. The first group goes in the passenger
compartment of the sleigh, and the second and third go in containers on either
side. Only when all three groups weigh exactly the same amount will the sleigh
be able to fly. Defying physics has rules, you know!

Of course, that's not the only problem. The first group - the one going in the
passenger compartment - needs as few packages as possible so that Santa has some
legroom left over. It doesn't matter how many packages are in either of the
other two groups, so long as all of the groups weigh the same.

Furthermore, Santa tells you, if there are multiple ways to arrange the packages
such that the fewest possible are in the first group, you need to choose the way
where the first group has the smallest quantum entanglement to reduce the chance
of any "complications". The quantum entanglement of a group of packages is the
product of their weights, that is, the value you get when you multiply their
weights together. Only consider quantum entanglement if the first group has the
fewest possible number of packages in it and all groups weigh the same amount.

For example, suppose you have ten packages with weights 1 through 5 and 7
through 11. For this situation, some of the unique first groups, their quantum
entanglements, and a way to divide the remaining packages are as follows:

Group 1;             Group 2; Group 3
11 9       (QE= 99); 10 8 2;  7 5 4 3 1
10 9 1     (QE= 90); 11 7 2;  8 5 4 3
10 8 2     (QE=160); 11 9;    7 5 4 3 1
10 7 3     (QE=210); 11 9;    8 5 4 2 1
10 5 4 1   (QE=200); 11 9;    8 7 3 2
10 5 3 2   (QE=300); 11 9;    8 7 4 1
10 4 3 2 1 (QE=240); 11 9;    8 7 5
9 8 3      (QE=216); 11 7 2;  10 5 4 1
9 7 4      (QE=252); 11 8 1;  10 5 3 2
9 5 4 2    (QE=360); 11 8 1;  10 7 3
8 7 5      (QE=280); 11 9;    10 4 3 2 1
8 5 4 3    (QE=480); 11 9;    10 7 2 1
7 5 4 3 1  (QE=420); 11 9;    10 8 2
Of these, although 10 9 1 has the smallest quantum entanglement (90), the
configuration with only two packages, 11 9, in the passenger compartment gives
Santa the most legroom and wins. In this situation, the quantum entanglement for
the ideal configuration is therefore 99. Had there been two configurations with
only two packages in the first group, the one with the smaller quantum
entanglement would be chosen.

What is the quantum entanglement of the first group of packages in the ideal
configuration?
*/

#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <format>
#include <functional>
#include <numeric>
#include <print>
#include <unordered_set>
#include <vector>

constexpr int kSizeOfInput = 29;

using Bitset = std::bitset<kSizeOfInput>;

// Function to pretty print an unordered set of integers
void PrettyPrintSet(const std::unordered_set<int>& set) {
  std::string result = "{";

  // Iterate over the set and append elements to the result string
  for (auto it = set.begin(); it != set.end(); ++it) {
    result += std::format("{}", *it);
    if (std::next(it) != set.end()) {
      result += ", ";
    }
  }

  result += "}";

  // Use std::print to print the formatted output
  std::print("{}\n", result);
}

int Sum(const std::vector<int>& packages, const Bitset& selected) {
  int sum = 0;
  for (int i = 0; i < packages.size(); ++i) {
    if (selected.test(i)) {
      sum += packages[i];
    }
  }
  return sum;
}
std::unordered_set<int> GetSet(const std::vector<int>& packages,
                               const Bitset& selected) {
  std::unordered_set<int> set;
  for (int i = 0; i < packages.size(); ++i) {
    if (selected.test(i)) {
      set.insert(packages[i]);
    }
  }
  return set;
}

void FindSolution(const std::vector<int>& packages, int target,
                  Bitset& selected, int candidate_idx,
                  std::unordered_set<int>& best) {
  // std::string dbg = selected.to_string();
  // std::reverse(dbg.begin(), dbg.end());
  // std::print("Testing: {}\n", dbg);
  int num_selected = selected.count();
  if (!best.empty() && num_selected > best.size()) {
    // We've already found a better solution.
    return;
  }
  int weight = Sum(packages, selected);
  if (weight > target) {
    // This isn't viable, we're overweight.
    return;
  }

  if (weight == target) {
    if (best.empty() || num_selected < best.size()) {
      // we found a new best!
      std::print("New best found, with only {} packages.\n", num_selected);
      best = GetSet(packages, selected);
      PrettyPrintSet(best);
    }
    std::unordered_set<int> candidate_set = GetSet(packages, selected);
    // If we get here, we have a candidate of equal size. Evaluate quantum
    // entanglement score.
    int64_t best_qe = std::accumulate(best.begin(), best.end(), 1l,
                                      std::multiplies<int64_t>());
    int64_t candidate_qe =
        std::accumulate(candidate_set.begin(), candidate_set.end(), 1l,
                        std::multiplies<int64_t>());
    if (candidate_qe < best_qe) {
      // overwrite the best.
      std::print(
          "New best found, still with {} packages, but a lower qe: {} vs. "
          "{}.\n",
          num_selected, candidate_qe, best_qe);
      best = candidate_set;
      PrettyPrintSet(best);
    } else {
      //      std::print(
      //          "New tie found, still with {} packages, but a higher qe: {}
      //          vs. "
      //          "{}.\n",
      //          num_selected, candidate_qe, best_qe);
      //      PrettyPrintSet(candidate_set);
    }
    // No need to proceed.
    return;
  }

  if (candidate_idx >= packages.size()) {
    // There are no more packages to consider.
    return;
  }

  // If we get here, the weight is < target, and we need to keep searching.
  // Select this package and recurse.
  selected.set(candidate_idx);
  FindSolution(packages, target, selected, candidate_idx + 1, best);

  // Ok now try without this package selected.
  selected.reset(candidate_idx);
  FindSolution(packages, target, selected, candidate_idx + 1, best);
}
int main() {
  // Just copy input data here.
  std::vector<int> packages = {1,  2,  3,  5,  7,   13,  17,  19,  23, 29,
                               31, 37, 41, 43, 53,  59,  61,  67,  71, 73,
                               79, 83, 89, 97, 101, 103, 107, 109, 113};

  // For all groups to be equal in weight, each group _must_ weigh exactly
  // 1/3 of the total.
  const int target_weight =
      std::accumulate(packages.begin(), packages.end(), 0) / 3;

  // Put the packages in reverse order to find the smallest groups first.
  std::reverse(packages.begin(), packages.end());

  std::unordered_set<int> best_solution;
  Bitset selected;

  FindSolution(packages, target_weight, selected, /*candidate_idx=*/0,
               best_solution);

  int64_t best_qe = std::accumulate(best_solution.begin(), best_solution.end(),
                                    1l, std::multiplies<int64_t>());

  std::print("Part 1 - Best QE: {}\n", best_qe);

  const int new_target_weight =
      std::accumulate(packages.begin(), packages.end(), 0) / 4;

  best_solution.clear();
  selected.reset();

  FindSolution(packages, new_target_weight, selected, 0, best_solution);

  best_qe = std::accumulate(best_solution.begin(), best_solution.end(), 1l,
                            std::multiplies<int64_t>());

  std::print("Part 2 - Best QE: {}\n", best_qe);

  return 0;
}
