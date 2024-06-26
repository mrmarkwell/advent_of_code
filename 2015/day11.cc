/*
--- Day 11: Corporate Policy ---
Santa's previous password expired, and he needs help choosing a new one.

To help him remember his new password after the old one expires, Santa has
devised a method of coming up with a password based on the previous one.
Corporate policy dictates that passwords must be exactly eight lowercase letters
(for security reasons), so he finds his new password by incrementing his old
password string repeatedly until it is valid.

Incrementing is just like counting with numbers: xx, xy, xz, ya, yb, and so on.
Increase the rightmost letter one step; if it was z, it wraps around to a, and
repeat with the next letter to the left until one doesn't wrap around.

Unfortunately for Santa, a new Security-Elf recently started, and he has imposed
some additional password requirements:

Passwords must include one increasing straight of at least three letters, like
abc, bcd, cde, and so on, up to xyz. They cannot skip letters; abd doesn't
count. Passwords may not contain the letters i, o, or l, as these letters can be
mistaken for other characters and are therefore confusing. Passwords must
contain at least two different, non-overlapping pairs of letters, like aa, bb,
or zz. For example:

hijklmmn meets the first requirement (because it contains the straight hij) but
fails the second requirement requirement (because it contains i and l). abbceffg
meets the third requirement (because it repeats bb and ff) but fails the first
requirement. abbcegjk fails the third requirement, because it only has one
double letter (bb). The next password after abcdefgh is abcdffaa. The next
password after ghijklmn is ghjaabcc, because you eventually skip all the
passwords that start with ghi..., since i is not allowed.

Given Santa's current password (your puzzle input), what should his next
password be?



--- Part Two ---
Santa's password expired again. What's the next one?
*/

#include <cassert>
#include <iostream>

using std::cout, std::endl, std::string;

string IncrementString(string str) {
  int n = str.size();

  // Start from the end of the string
  for (int i = n - 1; i >= 0; i--) {
    if (str[i] == 'z') {
      str[i] = 'a';  // Increment from 'z' to 'a'
    } else {
      str[i]++;    // Increment the current character.
      return str;  // Return the string.
    }
  }

  // If we reached here, all characters were 'z'
  str = 'a' + str;  // Prepend 'a' for the final increment.
  return str;
}

bool CheckForRunOf3(const string& str, int starting_pos) {
  // If there aren't enough charaters for a run we return.
  if (starting_pos + 2 >= str.length()) {
    return false;
  }
  // Check for the run.
  if (str[starting_pos] + 1 == str[starting_pos + 1] &&
      str[starting_pos] + 2 == str[starting_pos + 2]) {
    return true;
  } else {
    return false;
  }
}

bool CheckForPair(const string& str, int starting_pos) {
  // If there aren't enough characters left to make a pair we return.
  if (starting_pos + 1 >= str.length()) {
    return false;
  }
  // Check if we have a pair.
  if (str[starting_pos] == str[starting_pos + 1]) {
    return true;
  } else {
    return false;
  }
}

bool CheckForInvalidLetter(char letter) {
  // Checks if the passed in letter is one of the invalid letters.
  if (letter == 'i' || letter == 'o' || letter == 'l') {
    return true;
  } else {
    return false;
  }
}

string FindNextValidPassword(string str) {
  bool valid_password = false;

  // While we don't have a valid password, we continue the search.
  while (!valid_password) {
    // Increment to the next password for us to check.
    str = IncrementString(str);

    int pair_count = 0;
    int last_pair_pos = -20;
    bool run_of_3 = false;
    bool pairs = false;
    bool valid_chars = true;

    // Iterates over each letter of the password.
    for (int i = 0; i < str.length(); i++) {
      // If we find an invalid letter, we break out of the loop to go ahead to
      // the next possible password.
      if (CheckForInvalidLetter(str[i])) {
        valid_chars = false;
        break;
      }
      // We check from the current position in the string to see if there is a
      // run.
      if (CheckForRunOf3(str, i)) {
        run_of_3 = true;
      }
      // We check from the current position in the string to see if there is a
      // pair.
      if (CheckForPair(str, i) && i - last_pair_pos > 1) {
        pair_count++;
        last_pair_pos = i;
        if (pair_count >= 2) {
          pairs = true;
        }
      }
    }
    // If the current password meets all of the requirements for a valid
    // password, we set valid_password to true to break the while loop and
    // return the current password.
    if (run_of_3 && pairs && valid_chars) {
      valid_password = true;
    }
  }
  return str;
}

int main() {
  string initial_password = "hxbxwxba";
  assert(IncrementString("abc") == "abd");
  assert(IncrementString("azz") == "baa");
  assert(IncrementString("zzz") == "aaaa");
  assert(IncrementString("bczzz") == "bdaaa");

  assert(CheckForRunOf3("abc", 0) == true);
  assert(CheckForRunOf3("abe", 0) == false);
  assert(CheckForRunOf3("abc", 3) == false);
  assert(CheckForRunOf3("abce", 1) == false);
  assert(CheckForRunOf3("abce", 2) == false);
  assert(CheckForRunOf3("aatfkwxyal", 5) == true);
  assert(CheckForRunOf3("aatfkwxyal", 4) == false);

  assert(CheckForPair("abb", 1) == true);
  assert(CheckForPair("abb", 0) == false);
  assert(CheckForPair("abcdee", 4) == true);
  assert(CheckForPair("abc", 2) == false);
  assert(CheckForPair("ajfeddwdewkcew", 4) == true);

  assert(FindNextValidPassword("abcdefgh") == "abcdffaa");
  assert(FindNextValidPassword("ghijklmn") == "ghjaabcc");

  string next_password = FindNextValidPassword(initial_password);

  cout << "The next valid password after " << initial_password << ": "
       << next_password << endl;

  initial_password = next_password;
  next_password = FindNextValidPassword(initial_password);

  cout << "The next valid password after " << initial_password << ": "
       << next_password << endl;
  return 0;
}