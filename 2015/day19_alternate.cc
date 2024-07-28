/*
--- Day 19: Medicine for Rudolph ---
Rudolph the Red-Nosed Reindeer is sick! His nose isn't shining very brightly,
and he needs medicine.

Red-Nosed Reindeer biology isn't similar to regular reindeer biology; Rudolph is
going to need custom-made medicine. Unfortunately, Red-Nosed Reindeer chemistry
isn't similar to regular reindeer chemistry, either.

The North Pole is equipped with a Red-Nosed Reindeer nuclear fusion/fission
plant, capable of constructing any Red-Nosed Reindeer molecule you need. It
works by starting with some input molecule and then doing a series of
replacements, one per step, until it has the right molecule.

However, the machine has to be calibrated before it can be used. Calibration
involves determining the number of molecules that can be generated in one step
from a given starting point.

For example, imagine a simpler machine that supports only the following
replacements:

H => HO
H => OH
O => HH
Given the replacements above and starting with HOH, the following molecules
could be generated:

HOOH (via H => HO on the first H).
HOHO (via H => HO on the second H).
OHOH (via H => OH on the first H).
HOOH (via H => OH on the second H).
HHHH (via O => HH).
So, in the example above, there are 4 distinct molecules (not five, because HOOH
appears twice) after one replacement from HOH. Santa's favorite molecule,
HOHOHO, can become 7 distinct molecules (over nine replacements: six from H, and
three from O).

The machine replaces without regard for the surrounding characters. For example,
given the string H2O, the transition H => OO would result in OO2O.

Your puzzle input describes all of the possible replacements and, at the bottom,
the medicine molecule for which you need to calibrate the machine. How many
distinct molecules can be created after all the different ways you can do one
replacement on the medicine molecule?

--- Part Two ---
Now that the machine is calibrated, you're ready to begin molecule fabrication.

Molecule fabrication always begins with just a single electron, e, and applying
replacements one at a time, just like the ones during calibration.

For example, suppose you have the following replacements:

e => H
e => O
H => HO
H => OH
O => HH
If you'd like to make HOH, you start with e, and then make the following
replacements:

e => O to get O
O => HH to get HH
H => OH (on the second H) to get HOH
So, you could make HOH after 3 steps. Santa's favorite molecule, HOHOHO, can be
made in 6 steps.

How long will it take to make the medicine? Given the available replacements and
the medicine molecule in your puzzle input, what is the fewest number of steps
to go from e to the medicine molecule?
*/
#include <fmt/core.h>

#include <map>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "absl/strings/cord.h"
#include "utils/utils.h"

std::multimap<std::string, std::string> GetSubstitutions(
    std::vector<std::string> lines) {
  constexpr int kKey = 1;
  constexpr int kValue = 2;

  // All the names we have seen so far.
  std::multimap<std::string, std::string> substitutions;

  std::regex pattern(R"((\w+) => (\w+))");
  for (const auto& line : lines) {
    std::smatch match;
    if (!std::regex_search(line, match, pattern)) {
      continue;
    }

    substitutions.emplace(match[kKey].str(), match[kValue].str());
  }
  return substitutions;
}

int CountUniqueMolecules(
    const std::string& molecule,
    const std::multimap<std::string, std::string>& substitutions) {
  std::unordered_set<std::string> unique_molecules;

  // For each substitution, do it at every spot in the string.
  for (const auto& [key, value] : substitutions) {
    size_t start_pos = 0;

    while ((start_pos = molecule.find(key, start_pos)) != std::string::npos) {
      std::string new_molecule = molecule;
      new_molecule.replace(start_pos, key.length(), value);
      start_pos++;
      unique_molecules.insert(new_molecule);
      // fmt::print("K: {} V: {} M: {}\n", key, value, new_molecule);
    }
  }

  return unique_molecules.size();
}

// Reverses a std::multimap into a std::map.
std::unordered_map<std::string, std::string> ReverseMap(
    std::multimap<std::string, std::string> map) {
  std::unordered_map<std::string, std::string> reverse_map;
  for (const auto& [key, value] : map) {
    reverse_map.insert(std::make_pair(value, key));
  }
  return reverse_map;
}

// Function to replace the first occurrence of a substring in an absl::Cord
bool ReplaceFirstOccurrenceInPlace(absl::Cord& cord,
                                   const std::string& to_replace,
                                   const std::string& replacement) {
  std::string temp_string = std::string(cord);
  size_t pos = temp_string.find(to_replace);

  if (pos != std::string::npos) {
    absl::Cord before = cord.Subcord(0, pos);
    absl::Cord after = cord.Subcord(pos + to_replace.length(),
                                    cord.size() - (pos + to_replace.length()));

    absl::Cord result = before;
    result.Append(replacement);
    result.Append(after);

    cord = result;
    return true;
  }
  return false;
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day19_alternate.txt");

  std::string molecule = lines.back();
  std::multimap<std::string, std::string> substitutions =
      GetSubstitutions(lines);
  fmt::print("Unique Molecules: {}",
             CountUniqueMolecules(molecule, substitutions));

  // Make a reverse substitution map.
  std::unordered_map<std::string, std::string> reverse_map =
      ReverseMap(substitutions);

  absl::Cord molecule_cord(molecule);

  int steps = 0;
  while (molecule_cord != "e") {
    // NOTE: This should do backtracking when it reaches a dead end.
    // Alternatively, it could add some randomness.
    // I didn't need to since it worked first try for my input.
    for (const auto& [key, value] : reverse_map) {
      if (ReplaceFirstOccurrenceInPlace(molecule_cord, key, value)) {
        steps++;
        fmt::print("{}: {}\n", steps, molecule_cord.Flatten());
      }
    }
  }

  return 0;
}
