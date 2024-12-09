#include <print>

#include <regex>
#include <unordered_map>
#include <vector>

#include "utils/utils.h"

// Pretty print function for aunt_clues
void PrintAuntClues(
    const std::vector<std::unordered_map<std::string, int>>& aunt_clues) {
  int aunt_number = 1;
  for (const auto& aunt : aunt_clues) {
    std::print("Aunt {}: {{", aunt_number);
    for (auto it = aunt.begin(); it != aunt.end(); ++it) {
      std::print("{}: {}", it->first, it->second);
      if (std::next(it) != aunt.end()) {
        std::print(", ");
      }
    }
    std::print("}}\n");
    aunt_number++;
  }
}

std::vector<std::unordered_map<std::string, int>> GetAuntClues(
    const std::vector<std::string>& lines) {
  std::vector<std::unordered_map<std::string, int>> aunt_clues;
  std::regex pattern(R"(\w+ \d+: (\w+): (\d+), (\w+): (\d+), (\w+): (\d+))");

  for (const auto& line : lines) {
    std::unordered_map<std::string, int> current_aunt;
    std::smatch match;
    std::regex_search(line, match, pattern);

    for (int i = 1; i <= 6; i += 2) {
      current_aunt.insert(
          std::pair<std::string, int>(match[i], std::stoi(match[i + 1])));
    }
    aunt_clues.push_back(current_aunt);
  }
  return aunt_clues;
}

int HireSherlockHolmesToDeduceWhichAuntMyGiftIsFrom(
    const std::vector<std::unordered_map<std::string, int>>& aunt_clues) {
  const std::vector<std::string> clues = {
      "children", "cats",     "samoyeds", "pomeranians", "akitas",
      "vizslas",  "goldfish", "trees",    "cars",        "perfumes"};
  const std::unordered_map<std::string, int> mystery_aunt = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};

  for (int i = 0; i < aunt_clues.size(); i++) {
    int match_level = 0;
    for (auto clue : clues) {
      if (aunt_clues[i].find(clue) != aunt_clues[i].end()) {
        if (aunt_clues[i].at(clue) == mystery_aunt.at(clue)) {
          match_level++;
        }
      }
      if (match_level == 3) {
        return i + 1;
      }
    }
  }
  return -1;
}

bool CheckForMatch(const std::unordered_map<std::string, int>& aunt,
                   const std::unordered_map<std::string, int>& mystery_aunt,
                   const std::string& clue) {
  if (clue == "cats" || clue == "trees") {
    if (aunt.at(clue) <= mystery_aunt.at(clue)) {
      return false;
    }
  } else if (clue == "pomeranians" || clue == "goldfish") {
    if (aunt.at(clue) >= mystery_aunt.at(clue)) {
      return false;
    }
  } else {
    if (aunt.at(clue) != mystery_aunt.at(clue)) {
      return false;
    }
  }
  return true;
}

int ProvideSherlockHolmesWithTheNewEvidenceSoHeCanReevaluateTheCase(
    const std::vector<std::unordered_map<std::string, int>>& aunt_clues) {
  std::vector<std::string> clues = {
      "trees",  "children", "cats",     "samoyeds", "pomeranians",
      "akitas", "vizslas",  "goldfish", "cars",     "perfumes"};
  const std::unordered_map<std::string, int>& mystery_aunt = {
      {"children", 3}, {"cats", 7},    {"samoyeds", 2}, {"pomeranians", 3},
      {"akitas", 0},   {"vizslas", 0}, {"goldfish", 5}, {"trees", 3},
      {"cars", 2},     {"perfumes", 1}};

  for (int i = 0; i < aunt_clues.size(); i++) {
    int match_level = 0;
    for (auto clue : clues) {
      if (aunt_clues[i].find(clue) != aunt_clues[i].end()) {
        bool match = CheckForMatch(aunt_clues[i], mystery_aunt, clue);
        if (match) {
          match_level++;
        }
      }
      if (match_level == 3) {
        return i + 1;
      }
    }
  }
  return -1;
}

int main() {
  const std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day16.txt");
  const std::vector<std::unordered_map<std::string, int>> aunt_clues =
      GetAuntClues(lines);

  int correct_aunt =
      HireSherlockHolmesToDeduceWhichAuntMyGiftIsFrom(aunt_clues);

  std::print(
      "I, Sherlock Holmes have deduced that Aunt Sue {} sent you your awesome "
      "present!\n",
      correct_aunt);

  correct_aunt =
      ProvideSherlockHolmesWithTheNewEvidenceSoHeCanReevaluateTheCase(
          aunt_clues);

  std::print(
      "After recieving your email regarding the new evidence, I concluded that "
      "Aunt Sue {} sent you your present.\n",
      correct_aunt);
}