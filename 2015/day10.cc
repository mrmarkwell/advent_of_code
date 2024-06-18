/*
--- Day 10: Elves Look, Elves Say ---
Today, the Elves are playing a game called look-and-say. They take turns making
sequences by reading aloud the previous sequence and using that reading as the
next sequence. For example, 211 is read as "one two, two ones", which becomes
1221 (1 2, 2 1s).

Look-and-say sequences are generated iteratively, using the previous value as
input for the next step. For each step, take the previous value, and replace
each run of digits (like 111) with the number of digits (3) followed by the
digit itself (1).

For example:

1 becomes 11 (1 copy of digit 1).
11 becomes 21 (2 copies of digit 1).
21 becomes 1211 (one 2 followed by one 1).
1211 becomes 111221 (one 1, one 2, and two 1s).
111221 becomes 312211 (three 1s, two 2s, and one 1).

Starting with the digits in your puzzle input, apply this process 40 times. What
is the length of the result?





--- Part Two ---
Neat, right? You might also enjoy hearing John Conway talking about this
sequence (that's Conway of Conway's Game of Life fame).

Now, starting again with the digits in your puzzle input, apply this process 50
times. What is the length of the new result?
*/

#include <cassert>
#include <iostream>

using std::string, std::cout, std::to_string, std::endl;

string GetNextSequence(const string& current_sequence) {
  string new_sequence = "";
  if (current_sequence.empty()) {
    return "";
  }

  char previous_char = current_sequence[0];
  int count = 1;

  for (int i = 1; i < current_sequence.length(); ++i) {
    if (current_sequence[i] == previous_char) {
      count++;
    } else {
      new_sequence.append(to_string(count));
      new_sequence.push_back(previous_char);

      // Reset for the new character
      previous_char = current_sequence[i];
      count = 1;
    }
  }
  new_sequence.append(to_string(count));
  new_sequence.push_back(previous_char);

  return new_sequence;
}

void CopyToClipboard(const std::string& text) {
  // Create the command string
  std::string command = "echo \"" + text + "\" | pbcopy";
  // Execute the command
  system(command.c_str());
}

int main() {
  string sequence = "3113322113";

  assert(GetNextSequence("1") == "11");
  assert(GetNextSequence("11") == "21");
  assert(GetNextSequence("21") == "1211");
  assert(GetNextSequence("1211") == "111221");
  assert(GetNextSequence("111221") == "312211");

  string test = "1";
  for (int i = 0; i < 5; i++) {
    test = GetNextSequence(test);
  }

  assert(test == "312211");

  for (int i = 0; i < 40; i++) {
    sequence = GetNextSequence(sequence);
  }

  cout << "The final sequence length after 40 repititions is: "
       << sequence.length() << endl;

  for (int i = 0; i < 10; i++) {
    sequence = GetNextSequence(sequence);
  }

  cout << "The final sequence length after 50 repititions is: "
       << sequence.length() << endl;

  return 0;
}