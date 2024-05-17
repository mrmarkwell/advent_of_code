/*
--- Day 4: The Ideal Stocking Stuffer ---
Santa needs help mining some AdventCoins (very similar to bitcoins) to use as
gifts for all the economically forward-thinking little girls and boys.

To do this, he needs to find MD5 hashes which, in hexadecimal, start with at
least five zeroes. The input to the MD5 hash is some secret key (your puzzle
input, given below) followed by a number in decimal. To mine AdventCoins, you
must find Santa the lowest positive number (no leading zeroes: 1, 2, 3, ...)
that produces such a hash.

For example:

- If your secret key is abcdef, the answer is 609043, because the MD5 hash of
abcdef609043 starts with five zeroes (000001dbbfa...), and it is the lowest such
number to do so.
- If your secret key is pqrstuv, the lowest number it combines with to make an
MD5 hash starting with five zeroes is 1048970; that is, the MD5 hash of
pqrstuv1048970 looks like 000006136ef.... Your puzzle input is iwrupvqb.

--- Part Two ---
Now find one that starts with six zeroes.
*/

#include <openssl/md5.h>  // This is provided by BoringSSL

#include <cassert>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "utils/utils.h"

std::string ComputeMd5(const std::string& str) {
  // Create an MD5 digest context
  MD5_CTX ctx;
  MD5_Init(&ctx);

  // Compute the MD5 hash
  MD5_Update(&ctx, str.data(), str.size());

  // Finalize the MD5 hash
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5_Final(digest, &ctx);

  // Convert the digest to a hexadecimal string
  std::ostringstream oss;
  for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
    oss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
  }

  return oss.str();
}

int FindAdventCoin(std::string secret_key, int num_of_zeroes) {
  for (int i = 1;; ++i) {
    bool valid_coin = true;
    std::string hash = ComputeMd5(secret_key + std::to_string(i));
    for (int j = 0; j < num_of_zeroes; j++) {
      if (hash[j] != '0') {
        valid_coin = false;
      }
    }
    if (valid_coin) {
      return i;
    }
  }
}

int main() {
  std::string key = "yzbqklnj";
  int num_starting_zeroes = 5;

  // Go ahead and reserve space for appending numeric values.
  key.reserve(20);

  assert(FindAdventCoin("abcdef", num_starting_zeroes) == 609043);
  assert(FindAdventCoin("pqrstuv", num_starting_zeroes) == 1048970);

  std::cout << "5 zeroes: " << FindAdventCoin(key, num_starting_zeroes) << "\n";
  num_starting_zeroes = 6;
  std::cout << "6 zeroes: " << FindAdventCoin(key, num_starting_zeroes) << "\n";

  return 0;
}
