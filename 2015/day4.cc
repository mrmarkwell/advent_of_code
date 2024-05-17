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
*/

#include <openssl/md5.h>  // This is provided by BoringSSL

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

int main() {
  std::string key = "iwrupvqb";

  // Go ahead and reserve space for appending numeric values.
  key.reserve(20);

  for (int i = 0;; ++i) {
    (void)i;
    break;
  }

  std::cout << "MD5: " << ComputeMd5("abcdef6090431") << "\n";
  std::cout << "MD5: " << ComputeMd5("iwrupvqb1") << "\n";
  std::cout << "MD5: " << ComputeMd5("iwrupvqb2") << "\n";
  std::cout << "MD5: " << ComputeMd5("iwrupvqb3") << "\n";
  std::cout << "MD5: " << ComputeMd5("iwrupvqb4") << "\n";

  return 0;
}
