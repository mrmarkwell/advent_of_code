#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>

#include "utils/utils.h"

int NumOfNiceStrings(std::vector<std::string> inputs) {
  int nice_count = 0;
  for (const auto& input : inputs) {
    if (input.find("ab") != std::string::npos ||
        input.find("cd") != std::string::npos ||
        input.find("pq") != std::string::npos ||
        input.find("xy") != std::string::npos) {
      continue;
    }

    int vowel_count = 0;
    bool has_double = false;

    for (int i = 0; i < input.length(); i++) {
      if (i + 1 < input.length()) {
        if (input[i] == input[i + 1]) {
          has_double = true;
        }
      }
      if (input[i] == 'a' || input[i] == 'e' || input[i] == 'i' ||
          input[i] == 'o' || input[i] == 'u') {
        vowel_count++;
      }
      if (has_double && vowel_count >= 3) {
        nice_count++;
        break;
      }
    }
  }
  return nice_count;
}

int NumOfBetterNiceStrings(std::vector<std::string> inputs) {
  int nice_count = 0;
  for (const auto& input : inputs) {
    std::unordered_map<std::string, std::vector<int>> pair_starting_positions;
    bool has_separate_doubles = false;
    bool has_repeat = false;
    for (int i = 0; i < input.length(); i++) {
      if (i + 1 < input.length() && !has_separate_doubles) {
        std::string substr = input.substr(i, 2);
        auto it = pair_starting_positions.find(substr);
        if (it != pair_starting_positions.end()) {
          for (const auto& value : it->second) {
            if (i - value > 1) {
              has_separate_doubles = true;
            }
          }
        } else {
          pair_starting_positions.insert(
              std::pair<std::string, std::vector<int>>(substr, {i}));
        }
      }

      if (i + 2 < input.length()) {
        if (input[i + 2] == input[i]) {
          has_repeat = true;
        }
      }
      if (has_repeat && has_separate_doubles) {
        std::cout << input << ": ";
        nice_count++;
        std::cout << nice_count << "\n";
        break;
      }
    }
  }
  return nice_count;
}

int main() {
  std::vector<std::string> inputs =
      LoadStringsFromFile("./../../../../../../2015/day5.txt");

  assert(NumOfNiceStrings({"ugknbfddgicrmopn"}) == 1);
  assert(NumOfNiceStrings({"aaa"}) == 1);
  assert(NumOfNiceStrings({"jchzalrnumimnmhp"}) == 0);
  assert(NumOfNiceStrings({"haegwjzuvuyypxyu"}) == 0);
  assert(NumOfNiceStrings({"dvszwmarrgswjxmb"}) == 0);

  int count = NumOfNiceStrings(inputs);

  assert(NumOfBetterNiceStrings({"qjhvhtzxzqqjkmpb"}) == 1);
  assert(NumOfBetterNiceStrings({"xxyxx"}) == 1);
  assert(NumOfBetterNiceStrings({"uurcxstgmygtbstg"}) == 0);
  assert(NumOfBetterNiceStrings({"ieodomkazucvgmuy"}) == 0);
  assert(NumOfBetterNiceStrings({"cqfikbgxvjmnfncy"}) == 0);

  int better_count = NumOfBetterNiceStrings(inputs);

  std::cout << "Number of Better Nice Strings: " << better_count << "\n";
  std::cout << "Number of Nice Strings: " << count << "\n";

  return 0;
}