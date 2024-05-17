/*
--- Day 6: Probably a Fire Hazard ---
Because your neighbors keep defeating you in the holiday house decorating
contest year after year, you've decided to deploy one million lights in a
1000x1000 grid.

Furthermore, because you've been especially nice this year, Santa has mailed you
instructions on how to display the ideal lighting configuration.

Lights in your grid are numbered from 0 to 999 in each direction; the lights at
each corner are at 0,0, 0,999, 999,999, and 999,0. The instructions include
whether to turn on, turn off, or toggle various inclusive ranges given as
coordinate pairs. Each coordinate pair represents opposite corners of a
rectangle, inclusive; a coordinate pair like 0,0 through 2,2 therefore refers to
9 lights in a 3x3 square. The lights all start turned off.

To defeat your neighbors this year, all you have to do is set up your lights by
doing the instructions Santa sent you in order.

For example:

- turn on 0,0 through 999,999 would turn on (or leave on) every light.
- toggle 0,0 through 999,0 would toggle the first line of 1000 lights, turning
off the ones that were on, and turning on the ones that were off.
- turn off 499,499 through 500,500 would turn off (or leave off) the middle four
lights.
- After following the instructions, how many lights are lit?

--- Part Two ---
You just finish implementing your winning light pattern when you realize you
mistranslated Santa's message from Ancient Nordic Elvish.

The light grid you bought actually has individual brightness controls; each
light can have a brightness of zero or more. The lights all start at zero.

The phrase turn on actually means that you should increase the brightness of
those lights by 1.

The phrase turn off actually means that you should decrease the brightness of
those lights by 1, to a minimum of zero.

The phrase toggle actually means that you should increase the brightness of
those lights by 2.

What is the total brightness of all lights combined after following Santa's
instructions?

For example:

- turn on 0,0 through 0,0 would increase the total brightness by 1.
- toggle 0,0 through 999,999 would increase the total brightness by 2000000.


*/
#include <cassert>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>

#include "utils/utils.h"

enum class Action {
  kOn,
  kOff,
  kToggle,
};

struct CoordinateRect {
  int x1 = 0;
  int y1 = 0;
  int x2 = 0;
  int y2 = 0;
  Action action;
};

struct Light {
  bool on = false;
  int brightness = 0;
};

using Grid = std::vector<std::vector<Light>>;

Grid InitLightGrid() {
  constexpr int kGridSize = 1000;
  Grid grid(kGridSize);
  for (auto& vec : grid) {
    // Initialize all the lights to be off.
    vec = std::vector<Light>(kGridSize);
  }
  return grid;
}
// Function to convert action string to Action enum
Action StringToAction(const std::string& actionStr) {
  if (actionStr == "turn on") {
    return Action::kOn;
  } else if (actionStr == "turn off") {
    return Action::kOff;
  } else if (actionStr == "toggle") {
    return Action::kToggle;
  } else {
    throw std::invalid_argument("Unknown action: " + actionStr);
  }
}

// Function to parse a string and return a vector of CoordinateRect
std::vector<CoordinateRect> ParseInput(const std::string& input) {
  std::vector<CoordinateRect> result;
  std::regex re(R"((turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+))");
  std::smatch match;
  std::string::const_iterator searchStart(input.cbegin());

  while (std::regex_search(searchStart, input.cend(), match, re)) {
    CoordinateRect rect;
    rect.action = StringToAction(match[1].str());
    rect.x1 = std::stoi(match[2].str());
    rect.y1 = std::stoi(match[3].str());
    rect.x2 = std::stoi(match[4].str());
    rect.y2 = std::stoi(match[5].str());
    result.push_back(rect);
    searchStart = match.suffix().first;
  }

  return result;
}
std::vector<CoordinateRect> ParseCoordinates() {
  std::string inputs = aoc::ReadFileToString("./2015/day6.txt");

  return ParseInput(inputs);
}

void ApplyRect(Grid& grid, const CoordinateRect& rect) {
  for (int y = rect.y1; y <= rect.y2; ++y) {
    for (int x = rect.x1; x <= rect.x2; ++x) {
      Light& l = grid[x][y];
      switch (rect.action) {
        case Action::kOn: {
          l.on = true;
          ++l.brightness;
          break;
        }
        case Action::kOff: {
          l.on = false;
          l.brightness = std::max(0, l.brightness - 1);
          break;
        }
        case Action::kToggle: {
          l.on = !l.on;
          l.brightness += 2;
          break;
        }
      }
    }
  }
}

int CountBrightness(const Grid& grid) {
  int brightness = 0;
  for (int y = 0; y < grid.size(); ++y) {
    brightness += std::accumulate(grid[y].begin(), grid[y].end(), 0,
                                  [](int brightness, const Light& light) {
                                    return brightness + light.brightness;
                                  });
  }
  return brightness;
}

int CountOnLights(const Grid& grid) {
  int count = 0;
  for (int y = 0; y < grid.size(); ++y) {
    count += std::count_if(grid[y].begin(), grid[y].end(),
                           [](const Light& light) { return light.on; });
  }
  return count;
}

bool Test(std::string command, int expected_on, int expected_brightness) {
  Grid grid = InitLightGrid();
  auto rects = ParseInput(command);
  for (const auto& rect : rects) {
    ApplyRect(grid, rect);
  }
  return CountOnLights(grid) == expected_on &&
         CountBrightness(grid) == expected_brightness;
}

int main() {
  auto grid = InitLightGrid();

  std::vector<CoordinateRect> rects = ParseCoordinates();

  for (const auto& rec : rects) {
    ApplyRect(grid, rec);
  }

  assert(Test("turn on 0,0 through 999,999", 1000000, 1000000));
  assert(Test("toggle 0,0 through 999,0", 1000, 2000));
  assert(Test("turn on 499,499 through 500,500 turn on 499,499 through 500,500",
              4, 8));

  std::cout << "On count: " << CountOnLights(grid) << std::endl;
  std::cout << "Brightness: " << CountBrightness(grid) << std::endl;

  return 0;
}
