/*
--- Day 8: Matchsticks ---
Space on the sleigh is limited this year, and so Santa will be bringing his list
as a digital copy. He needs to know how much space it will take up when stored.

It is common in many programming languages to provide a way to escape special
characters in strings. For example, C, JavaScript, Perl, Python, and even PHP
handle special characters in very similar ways.

However, it is important to realize the difference between the number of
characters in the code representation of the string literal and the number of
characters in the in-memory string itself.

For example:

- "" is 2 characters of code (the two double quotes), but the string contains
zero characters.
- "abc" is 5 characters of code, but 3 characters in the string data.
- "aaa\"aaa" is 10 characters of code, but the string itself contains six "a"
characters and a single, escaped quote character, for a total of 7 characters in
the string data.
- "\x27" is 6 characters of code, but the string itself contains just one - an
apostrophe ('), escaped using hexadecimal notation.

Santa's list is a file that contains many double-quoted string literals, one on
each line. The only escape sequences used are \\ (which represents a single
backslash), \" (which represents a lone double-quote character), and \x plus two
hexadecimal characters (which represents a single character with that ASCII
code).

Disregarding the whitespace in the file, what is the number of characters of
code for string literals minus the number of characters in memory for the values
of the strings in total for the entire file?

For example, given the four strings above, the total number of characters of
string code (2 + 5 + 10 + 6 = 23) minus the total number of characters in memory
for string values (0 + 3 + 7 + 1 = 11) is 23 - 11 = 12.

--- Part Two ---
Now, let's go the other way. In addition to finding the number of characters of
code, you should now encode each code representation as a new string and find
the number of characters of the new encoded representation, including the
surrounding double quotes.

For example:

- "" encodes to "\"\"", an increase from 2 characters to 6.
- "abc" encodes to "\"abc\"", an increase from 5 characters to 9.
- "aaa\"aaa" encodes to "\"aaa\\\"aaa\"", an increase from 10 characters to 16.
- "\x27" encodes to "\"\\x27\"", an increase from 6 characters to 11.
Your task is to find the total number of characters to represent the newly
encoded strings minus the number of characters of code in each original string
literal. For example, for the strings above, the total encoded length (6 + 9 +
16 + 11 = 42) minus the characters in the original code representation (23, just
like in the first part of this puzzle) is 42 - 23 = 19.

*/
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "utils/utils.h"

size_t ComputeEncodedSize(std::string_view str) {
  size_t encoded_size = str.size();
  // This should always be true.
  if (str.starts_with("\"") && str.ends_with("\"")) {
    // The starting and ending quotes are special because they require two
    // characters to escape, but the resulting string has _another_ containing
    // quote, so they add 2 characters each.
    encoded_size += 4;
    // Strip off the quotes.
    str = str.substr(1, str.size() - 2);
  }

  encoded_size += std::count_if(str.begin(), str.end(), [](char c) {
    // These need to be escaped, so count them.
    return (c == '"' || c == '\\');
  });

  return encoded_size;
}

size_t ComputeInMemorySize(std::string_view str) {
  size_t in_memory_size = str.size();

  // Disregard the containing quotes.
  if (str.starts_with("\"") && str.ends_with("\"")) {
    in_memory_size -= 2;
  }

  constexpr std::string_view escape_char = R"(\)";

  // Find any backslash first.
  size_t pos = str.find(escape_char, 0);
  while (pos != std::string_view::npos) {
    if (str.size() - 1 == pos) {
      // This shouldn't happen but we don't want to read off the end of the
      // string.
      continue;
    }

    if (str[pos + 1] == 'x') {
      // if this is an \x sequence, it is 4 characters that represent 1 in
      // memory. Subtract the other three.
      in_memory_size -= 3;
    } else {
      // Any other escape value is 2 characters for 1 byte in memory.
      --in_memory_size;
    }
    // Continue the search, skipping over the next character.
    // This skip is important because "//" would match again, which we need to
    // avoid since the second backslash has already been accounted for.
    pos = str.find(escape_char, pos + 2);
  }
  return in_memory_size;
}

struct TotalSizes {
  size_t code_size = 0;
  size_t in_memory_size = 0;
  size_t encoded_size = 0;
};

TotalSizes ComputeSizes(std::vector<std::string>& strings) {
  TotalSizes sizes;
  // Strip whitespace first.
  aoc::StripWhitespace(strings);

  sizes.code_size = aoc::TotalSize(strings);
  sizes.in_memory_size = 0;
  for (const std::string& str : strings) {
    sizes.in_memory_size += ComputeInMemorySize(str);
    sizes.encoded_size += ComputeEncodedSize(str);
  }

  return sizes;
}

bool Test() {
  std::vector<std::string> strings =
      aoc::LoadStringsFromFileByLine("./2015/day8test.txt");
  TotalSizes sizes = ComputeSizes(strings);
  return sizes.code_size == 23 && sizes.in_memory_size == 11 &&
         sizes.encoded_size == 42;
}

int main() {
  assert(Test());

  std::vector<std::string> strings =
      aoc::LoadStringsFromFileByLine("./2015/day8.txt");
  TotalSizes sizes = ComputeSizes(strings);

  std::cout << "Code size = " << sizes.code_size
            << " In memory size = " << sizes.in_memory_size
            << " Encoded size = " << sizes.encoded_size << "\n";

  std::cout << "Original minus in-memory = "
            << (sizes.code_size - sizes.in_memory_size)
            << " Encoded minus original: "
            << (sizes.encoded_size - sizes.code_size) << "\n";

  return 0;
}
