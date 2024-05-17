#include <iostream>
#include "utils/utils.h"

int main() {
    std::vector<std::string> inputs = LoadStringsFromFile("./../../../../../../2015/day5.txt");

    for(const auto& input:inputs) {
        std::cout << input << "\n";
    }
    return 0;
}