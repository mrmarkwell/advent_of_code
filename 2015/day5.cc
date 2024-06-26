/*
--- Day 5: Doesn't He Have Intern-Elves For This? ---
Santa needs help figuring out which strings in his text file are naughty or
nice.

A nice string is one with all of the following properties:

It contains at least three vowels (aeiou only), like aei, xazegov, or
aeiouaeiouaeiou. It contains at least one letter that appears twice in a row,
like xx, abcdde (dd), or aabbccdd (aa, bb, cc, or dd). It does not contain the
strings ab, cd, pq, or xy, even if they are part of one of the other
requirements. For example:

- ugknbfddgicrmopn is nice because it has at least three vowels (u...i...o...),
a double letter (...dd...), and none of the disallowed substrings.
- aaa is nice because it has at least three vowels and a double letter, even
though the letters used by different rules overlap.
- jchzalrnumimnmhp is naughty because it has no double letter.
- haegwjzuvuyypxyu is naughty because it contains the string xy.
- dvszwmarrgswjxmb is naughty because it contains only one vowel.

How many strings are nice?

Your puzzle answer was 238.

--- Part Two ---
Realizing the error of his ways, Santa has switched to a better model of
determining whether a string is naughty or nice. None of the old rules apply, as
they are all clearly ridiculous.

Now, a nice string is one with all of the following properties:

It contains a pair of any two letters that appears at least twice in the string
without overlapping, like xyxy (xy) or aabcdefgaa (aa), but not like aaa (aa,
but it overlaps). It contains at least one letter which repeats with exactly one
letter between them, like xyx, abcdefeghi (efe), or even aaa. For example:

- qjhvhtzxzqqjkmpb is nice because is has a pair that appears twice (qj) and a
letter that repeats with exactly one letter between them (zxz).
- xxyxx is nice because it has a pair that appears twice and a letter that
repeats with one between, even though the letters used by each rule overlap.
- uurcxstgmygtbstg is naughty because it has a pair (tg) but no repeat with a
single letter between them.
- ieodomkazucvgmuy is naughty because it has a repeating letter with one between
(odo), but no pair that appears twice.

How many strings are nice under these new rules?

Your puzzle answer was 69.
*/
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

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
    std::unordered_map<std::string, int> pair_starting_positions;
    bool has_separate_doubles = false;
    bool has_repeat = false;
    for (int i = 0; i < input.length(); i++) {
      if (i + 1 < input.length() && !has_separate_doubles) {
        std::string substr = input.substr(i, 2);
        auto it = pair_starting_positions.find(substr);
        if (it != pair_starting_positions.end()) {
          if (i - it->second > 1) {
            has_separate_doubles = true;
          }
        } else {
          pair_starting_positions.insert(
              std::pair<std::string, int>(substr, {i}));
        }
      }

      if (i + 2 < input.length() && !has_repeat) {
        if (input[i + 2] == input[i]) {
          has_repeat = true;
        }
      }
      if (has_repeat && has_separate_doubles) {
        nice_count++;
        break;
      }
    }
  }
  return nice_count;
}

int main() {
  std::vector<std::string> inputs =
      aoc::LoadStringsFromFileByLine("./2015/day5.txt");

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
