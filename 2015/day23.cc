/*

--- Day 23: Opening the Turing Lock ---
Little Jane Marie just got her very first computer for Christmas from some
unknown benefactor. It comes with instructions and an example program, but the
computer itself seems to be malfunctioning. She's curious what the program does,
and would like you to help her run it.

The manual explains that the computer supports two registers and six
instructions (truly, it goes on to remind the reader, a state-of-the-art
technology). The registers are named a and b, can hold any non-negative integer,
and begin with a value of 0. The instructions are as follows:

hlf r sets register r to half its current value, then continues with the next
instruction. tpl r sets register r to triple its current value, then continues
with the next instruction. inc r increments register r, adding 1 to it, then
continues with the next instruction. jmp offset is a jump; it continues with the
instruction offset away relative to itself. jie r, offset is like jmp, but only
jumps if register r is even ("jump if even"). jio r, offset is like jmp, but
only jumps if register r is 1 ("jump if one", not odd). All three jump
instructions work with an offset relative to that instruction. The offset is
always written with a prefix + or - to indicate the direction of the jump
(forward or backward, respectively). For example, jmp +1 would simply continue
with the next instruction, while jmp +0 would continuously jump back to itself
forever.

The program exits when it tries to run an instruction beyond the ones defined.

For example, this program sets a to 2, because the jio instruction causes it to
skip the tpl instruction:

inc a
jio a, +2
tpl a
inc a
What is the value in register b when the program in your puzzle input is
finished executing?

--- Part Two ---

The unknown benefactor is very thankful for releasi-- er, helping little Jane
Marie with her computer. Definitely not to distract you, what is the value in
register b after the program is finished executing if register a starts as 1
instead?
*/
#include <fmt/core.h>

#include <string>
#include <vector>

#include "utils/utils.h"

// Function to check if a string starts with a given prefix
bool StartsWith(const std::string& str, const std::string& prefix) {
  if (prefix.size() > str.size()) {
    return false;
  }
  // Compare the prefix with the beginning of the string
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

// Function to check if a string contains a given substring
bool Contains(const std::string& str, const std::string& substring) {
  // Use std::string::find to check if the substring exists within the string
  return str.find(substring) != std::string::npos;
}
// Function to extract the last number from a string and return it as an integer
int GetLastNumberAsInt(const std::string& str) {
  std::string number;
  bool in_number = false;

  // Iterate over the string in reverse
  for (auto it = str.rbegin(); it != str.rend(); ++it) {
    if (std::isdigit(*it)) {
      // If the current character is a digit, prepend it to the number
      number.insert(number.begin(), *it);
      in_number = true;
    } else if (in_number) {
      // If a non-digit is encountered after finding digits, break the loop
      break;
    }
  }

  // Convert the number string to an integer, or return 0 if no number is found
  return number.empty() ? 0 : std::stoi(number);
}

// Apply offset to the instruction counter i based on the jump requested in the
// instruction.

void Jump(const std::string& instruction, int& i) {
  bool negative = Contains(instruction, "-");
  int distance = GetLastNumberAsInt(instruction);

  // negate the jump if necessary
  distance = negative ? (distance * -1) : distance;

  // Subtract one to account for the fact that the instruction pointer always
  // moves forward one each iteration.
  // E.g. a "jmp +0" should actually move the instruction pointer back one, so
  // when it increments it returns to the same instruction.
  distance -= 1;

  // Modify the instruction pointer.
  i += distance;
}

int main() {
  std::vector<std::string> instructions =
      aoc::LoadStringsFromFileByLine("./2015/day23.txt");

  // These have to be 64 bit (it overflowed with 32).
  int64_t a = 0;
  int64_t b = 0;

  // Given the instruction, returns the register (by reference) that this
  // instruction operates on.
  auto GetRegister = [&a, &b](const std::string& str) -> int64_t& {
    if (Contains(str, " a")) {
      return a;
    }
    return b;
  };

  for (int i = 0; i < instructions.size(); ++i) {
    std::string& instruction = instructions[i];
    fmt::print("Executing instruction {}. a[{}] b[{}] instruction: {}\n", i, a,
               b, instruction);
    // If this instruction requires a register, store a ref to it here.
    int64_t& reg = GetRegister(instruction);
    if (StartsWith(instruction, "hlf")) {
      reg /= 2;
    } else if (StartsWith(instruction, "tpl")) {
      reg *= 3;
    } else if (StartsWith(instruction, "inc")) {
      reg++;
    } else if (StartsWith(instruction, "jmp")) {
      Jump(instruction, i);
    } else if (StartsWith(instruction, "jie")) {
      if (reg % 2 == 0) {
        Jump(instruction, i);
      }
    } else if (StartsWith(instruction, "jio")) {
      if (reg == 1) {
        Jump(instruction, i);
      }
    }
  }
  fmt::print("The final value of the registers are a: {} b: {}\n", a, b);
  return 0;
}
