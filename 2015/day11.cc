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
  if (starting_pos + 2 >= str.length()) {
    return false;
  }
  if (str[starting_pos] + 1 == str[starting_pos + 1] &&
      str[starting_pos] + 2 == str[starting_pos + 2]) {
    return true;
  } else {
    return false;
  }
}

bool CheckForPair(const string& str, int starting_pos) {
  if (starting_pos + 1 >= str.length()) {
    return false;
  }
  if (str[starting_pos] == str[starting_pos + 1]) {
    return true;
  } else {
    return false;
  }
}

bool CheckForInvalidLetter(char letter) {
  if (letter == 'i' || letter == 'o' || letter == 'l') {
    return true;
  } else {
    return false;
  }
}

string FindNextValidPassword(string str) {
  bool valid_password = false;
  while (!valid_password) {
    str = IncrementString(str);
    int pair_count = 0;
    int last_pair_pos = -20;
    bool run_of_3 = false;
    bool pairs = false;
    for (int i = 0; i < str.length(); i++) {
      if (CheckForInvalidLetter(str[i])) {
        break;
      }
      if (CheckForRunOf3(str, i)) {
        run_of_3 = true;
      }
      if (CheckForPair(str, i) && i - last_pair_pos > 1) {
        pair_count++;
        last_pair_pos = i;
        if (pair_count >= 2) {
          pairs = true;
        }
      }
    }
    if (run_of_3 && pairs) {
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