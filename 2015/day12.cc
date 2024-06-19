/*
--- Day 12: JSAbacusFramework.io ---
Santa's Accounting-Elves need help balancing the books after a recent order.
Unfortunately, their accounting software uses a peculiar storage format. That's
where you come in.

They have a JSON document which contains a variety of things: arrays ([1,2,3]),
objects ({"a":1, "b":2}), numbers, and strings. Your first job is to simply find
all of the numbers throughout the document and add them together.

For example:

[1,2,3] and {"a":2,"b":4} both have a sum of 6.
[[[3]]] and {"a":{"b":4},"c":-1} both have a sum of 3.
{"a":[-1,1]} and [-1,{"a":1}] both have a sum of 0.
[] and {} both have a sum of 0.
You will not encounter any strings containing numbers.

What is the sum of all numbers in the document?

--- Part Two ---
Uh oh - the Accounting-Elves have realized that they double-counted everything
red.

Ignore any object (and all of its children) which has any property with the
value "red". Do this only for objects ({...}), not arrays ([...]).

[1,2,3] still has a sum of 6.
[1,{"c":"red","b":2},3] now has a sum of 4, because the middle object is
ignored.
{"d":"red","e":[1,2,3,4],"f":5} now has a sum of 0, because the entire structure
is ignored. [1,"red",5] has a sum of 6, because "red" in an array has no effect.
*/

#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>

#include "utils/utils.h"

using json = nlohmann::json;

// Function to recursively iterate through a JSON object
void IterateJson(const json& j, int& sum) {
  // If the current JSON object contains a key with the value "red", skip this
  // object
  if (j.is_object()) {
    for (const auto& item : j.items()) {
      if (item.value() == "red") {
        return;
      }
    }
  }

  // Iterate through the JSON object or array
  for (auto it = j.begin(); it != j.end(); ++it) {
    // Recursively process the next level if the current item is an object or
    // array
    if (it->is_object() || it->is_array()) {
      IterateJson(*it, sum);
    } else {
      // Print the key and value if it's a key-value pair in an object

      // If we get here, this object or array of values that should not be
      // excluded. Add up all the numberic values.
      auto& value = it.value();
      sum += value.is_number() ? static_cast<int>(value) : 0;
    }
  }
}

// Function to calculate the sum of all numbers in a string using std::regex
int SumOfNumbers(const std::string& input) {
  std::regex number_pattern(R"(-?\d+)");
  std::sregex_iterator current_match(input.begin(), input.end(),
                                     number_pattern);
  std::sregex_iterator last_match;

  int sum = 0;

  while (current_match != last_match) {
    std::smatch match = *current_match;
    sum += std::stoi(match.str());
    current_match++;
  }
  return sum;
}

int main() {
  std::string input = aoc::ReadFileToString("./2015/day12.txt");

  std::cout << "The sum of all the numbers is: " << SumOfNumbers(input)
            << std::endl;

  json json_object = json::parse(input);

  int sum = 0;

  IterateJson(json_object, sum);

  std::cout << "The sum of all the valid numbers (part 2) is: " << sum
            << std::endl;
}
