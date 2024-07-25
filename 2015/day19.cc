#include <fmt/core.h>

#include <cassert>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "utils/utils.h"

void PrettyPrintMap(
    const std::unordered_map<std::string, std::vector<std::string>>& map) {
  fmt::print("Map contents:\n");
  for (const auto& pair : map) {
    fmt::print("{}: ", pair.first);
    fmt::print("[");
    for (size_t i = 0; i < pair.second.size(); ++i) {
      fmt::print("{}", pair.second[i]);
      if (i < pair.second.size() - 1) {
        fmt::print(", ");
      }
    }
    fmt::print("]\n");
  }
}

std::unordered_map<std::string, std::vector<std::string>> ParseData(
    std::vector<std::string> lines, std::string& molecule_input) {
  std::unordered_map<std::string, std::vector<std::string>>
      molecule_modifications;

  std::regex pattern(R"((\w+) => (\w+))");
  std::regex pattern2(R"(([A-Za-z]+))");
  for (std::string& line : lines) {
    std::smatch match;
    if (std::regex_search(line, match, pattern)) {  // Check if match was found
      molecule_modifications[match[1]].push_back(match[2]);
    } else if (std::regex_search(line, match, pattern2)) {
      molecule_input = match[0];
    }
  }
  return molecule_modifications;
}

void ReplaceSubstring(const std::string& original, const std::string& to_search,
                      const std::string& replace_with,
                      std::unordered_set<std::string>& molecule_variants) {
  // Tracks the current position in the string.
  int pos = original.find(to_search);

  // While we are not at the end of the string...
  while (pos != std::string::npos) {
    std::string next_string = original;

    // Replaces the next substring that matches the one we are searching for
    // with our replacement.
    next_string.replace(pos, to_search.length(), replace_with);

    // If the variant does not exist in our list of variants we add it to the
    // list.
    molecule_variants.insert(next_string);

    // Increment to the next character after our substring to continue the
    // search.
    pos = original.find(to_search, pos + 1);
  }
}

std::unordered_set<std::string> GetNextDistinctMolecules(
    const std::unordered_map<std::string, std::vector<std::string>>&
        molecule_modifications,
    const std::string& initial_molecule) {
  std::unordered_set<std::string> molecule_variants;
  for (const auto& molecule_modification : molecule_modifications) {
    for (const std::string& modification : molecule_modification.second) {
      ReplaceSubstring(initial_molecule, molecule_modification.first,
                       modification, molecule_variants);
    }
  }
  return molecule_variants;
}

bool MatchingVariant(const std::string& desired_medicine,
                     const std::unordered_set<std::string>& mutations) {
  for (const std::string& mutation : mutations) {
    if (mutation == desired_medicine) {
      return true;
    }
  }
  return false;
}

int GenerateMedicineMolecule(
    const std::string& desired_molecule,
    std::unordered_map<std::string, std::vector<std::string>>
        molecule_modifications) {
  // Initial starting molecule.
  std::string medicine_molecule = "e";
  // Holds the molecule mutations.
  std::unordered_set<std::string> mutations = {medicine_molecule};
  // Tracks the number of loops/mutations.
  int num_of_mutations = 0;

  while (true) {
    std::unordered_set<std::string> next_mutations = {};
    if (MatchingVariant(desired_molecule, mutations)) {
      return num_of_mutations;
    }
    // Keep modifying molecule.
    for (auto& mutation : mutations) {
      next_mutations.merge(
          GetNextDistinctMolecules(molecule_modifications, mutation));
    }
    mutations = next_mutations;
    num_of_mutations++;
  }

  return num_of_mutations;
}

void TestPart1() {
  std::string starting_molecule = "HOH";
  std::unordered_map<std::string, std::vector<std::string>>
      molecule_modifications = {{"H", {"HO", "OH"}}, {"O", {"HH"}}};
  assert(GetNextDistinctMolecules(molecule_modifications, starting_molecule)
             .size() == 4);
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day19.txt");
  std::string medicine_molecule = "";
  std::unordered_map<std::string, std::vector<std::string>>
      molecule_modifications = ParseData(lines, medicine_molecule);

  std::unordered_set<std::string> distinct_molecules =
      GetNextDistinctMolecules(molecule_modifications, medicine_molecule);
  fmt::print("There are {} distinct molecule variations.\n",
             distinct_molecules.size());

  return 0;
}