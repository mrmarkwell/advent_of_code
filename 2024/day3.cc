#include <cassert>
#include <cctype>

#include "utils/utils.h"

void CheckForMulFunc(const std::string &input, int &sum, int &i) {
  // std::cout << input.substr(i, 4) << std::endl;
  if (input.substr(i, 4) != "mul(") {
    // std::cout << input.substr(i, 4) << std::endl;
    return;
  }
  i += 4;

  int num1 = 0;
  int num2 = 0;
  int digit_count = 0;
  int digit;
  for (int j = i; j < i + 4; j++) {
    digit_count++;
    if (std::isdigit(input[j])) {
      digit = input[j] - '0';
      num1 *= 10;
      num1 += digit;
    } else if (input[j] == ',') {
      i += digit_count;
      break;
    } else {
      return;
    }
  }

  if (num1 > 999) {
    return;
  }

  for (int j = i; j < i + 4; j++) {
    if (std::isdigit(input[j])) {
      digit = input[j] - '0';
      num2 *= 10;
      num2 += digit;
    } else if (input[j] == ')') {
      break;
    } else {
      return;
    }
  }
  if (num2 > 999) {
    return;
  }
  // std::cout << "NUM1: " << num1 << std::endl;
  // std::cout << "NUM2: " << num2 << "\n\n\n";
  sum += num1 * num2;
}

void CheckForSwitch(std::string input, bool &enabled, int &i) {
  if (input.substr(i, 7) == "don't()") {
    enabled = false;
    i += 6;
  } else if (input.substr(i, 4) == "do()") {
    enabled = true;
    i += 3;
  }
  return;
}

int CalculateSumP2(const std::string &input) {
  bool enabled = true;
  int sum = 0;
  for (int i = 0; i < input.size(); i++) {
    if (input[i] == 'd') {
      CheckForSwitch(input, enabled, i);
    } else if (input[i] == 'm' && enabled) {
      CheckForMulFunc(input, sum, i);
    }
  }

  return sum;
}

int CalculateSum(const std::string &input) {
  int sum = 0;
  for (int i = 0; i < input.size(); i++) {
    if (input[i] == 'm') {
      CheckForMulFunc(input, sum, i);
    }
  }

  return sum;
}

int main() {
  std::string input = aoc::ReadFileToString("./2024/day3.txt");
  // std::string input =
  //     "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";
  //  std::cout << "SUM: " << CalculateSum(input) << std::endl;
  //  assert(CalculateSum(input) == 161);
  std::cout << "Part 1: " << CalculateSum(input) << std::endl;
  std::cout << "Part 2: " << CalculateSumP2(input);
  return 0;
}