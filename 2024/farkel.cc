/*
 */

#include <algorithm>
#include <array>
#include <cstdint>
#include <print>
#include <random>
#include <vector>

#include "absl/log/check.h"
#include "utils/utils.h"

// There are 6 dice in this game.
constexpr int kNumDice = 6;
// Function to simulate a random die roll (1 to 6)
int RollDie() {
  // Static Mersenne Twister random number generator
  static thread_local std::mt19937 rng(std::random_device{}());

  // Define a uniform distribution for numbers 1 through 6
  static thread_local std::uniform_int_distribution<int> dist(1, 6);

  // Generate and return a random number
  return dist(rng);
}

class Die {
 public:
  // Roll the die if it is live.
  void Roll() {
    if (IsLive()) {
      value_ = RollDie();
    }
  }

  // States the die can be in.
  enum class State {
    kLive,
    kScored,
  };

  int Value() const { return value_; }
  bool IsLive() const { return state_ == State::kLive; }
  bool IsScored() const { return state_ == State::kScored; }

  // if the dice are hot, they need to be made live again and marked as not
  // scored.
  void Reset() { state_ = State::kLive; }
  void MarkScored() { state_ = State::kScored; }

  // Comparison operator for sorting
  bool operator<(const Die& other) const {
    // Compare based on value first; if equal, compare state
    if (value_ != other.value_) {
      return value_ < other.value_;
    }
    // States are ordered arbitrarily for sorting consistency
    return state_ < other.state_;
  }

 private:
  int value_ = 1;

  // Current state of the die.
  State state_ = State::kLive;
};

// alias to represent all the dice.
using Dice = std::array<Die, kNumDice>;

//*** Helper functions that operate on dice. ***//

// Returns the number of live dice.
int NumLiveDice(const Dice& dice) {
  return std::count_if(dice.cbegin(), dice.cend(),
                       [](const Die& die) { return die.IsLive(); });
}

// Make all dice live again.
void HotDice(Dice& dice) {
  std::for_each(dice.begin(), dice.end(), [](Die& die) { die.Reset(); });
}

void PrintDice(const std::array<Die, 6>& dice) {
  // Group dice values by their states
  std::vector<int> scored, live;

  for (const auto& die : dice) {
    if (die.IsScored()) {
      scored.push_back(die.Value());
    } else if (die.IsLive()) {
      live.push_back(die.Value());
    }
  }

  // Helper lambda to format a section directly with std::print
  auto format_section = [](const std::string& label,
                           const std::vector<int>& values, int width) {
    std::print("{} {{", label);
    for (int value : values) {
      std::print(" {}", value);
    }
    std::print(" }}");

    // Calculate remaining space for padding
    int printed_width = static_cast<int>(label.size()) + 3 +
                        static_cast<int>(values.size()) * 2;
    if (!values.empty()) {
      --printed_width;  // Account for the lack of a trailing space
    }
    int padding = width - printed_width;
    if (padding > 0) {
      std::print("{:{}}", "", padding);  // Print padding spaces
    }
  };

  // Define the fixed widths for each section
  constexpr int section_width = 20;

  // Print each section
  format_section("Scored", scored, section_width);
  format_section("Live", live, section_width);

  // Finish the line
  std::print("\n");
}

void MarkAllDiceScored(Dice& dice) {
  std::for_each(dice.begin(), dice.end(), [](Die& die) { die.MarkScored(); });
}

void MarkNumberAsScored(std::vector<Die*>& dice, int number_to_mark) {
  std::for_each(dice.begin(), dice.end(), [&number_to_mark](Die* die) {
    if (die->Value() == number_to_mark) {
      die->MarkScored();
    }
  });
}

// Score the dice. Dice will be used in scoring only if they are:
// 1. Not live.
// 2. Not scored.
int Score(Dice& dice) {
  std::vector<Die*> possibly_scoring_dice{};
  // Collect the dice that are not live and not scored.
  for (Die& die : dice) {
    if (!die.IsScored()) {
      possibly_scoring_dice.push_back(&die);
    }
  }

  // Count occurrences of each die value (1 to 6)
  std::array<int, 6> counts = {0, 0, 0, 0, 0, 0};
  for (Die* die : possibly_scoring_dice) {
    int value = die->Value();
    if (value >= 1 && value <= 6) {
      ++counts[value - 1];
    }
  }

  int score = 0;

  // Check for special patterns
  auto is_straight = [&]() -> bool {
    return std::all_of(counts.begin(), counts.end(),
                       [](int count) { return count == 1; });
  };

  auto is_three_pairs = [&]() -> bool {
    bool three_pairs = std::count_if(counts.begin(), counts.end(),
                                     [](int count) { return count == 2; }) == 3;
    bool four_and_two =
        std::count_if(counts.begin(), counts.end(),
                      [](int count) { return count == 4; }) == 1 &&
        std::count_if(counts.begin(), counts.end(),
                      [](int count) { return count == 2; }) == 1;
    return three_pairs && four_and_two;
  };

  auto is_two_triplets = [&]() -> bool {
    return std::count_if(counts.begin(), counts.end(),
                         [](int count) { return count == 3; }) == 2;
  };

  // Scoring for special patterns
  if (is_straight()) {
    MarkAllDiceScored(dice);
    return 1500;
  }

  if (is_three_pairs()) {
    MarkAllDiceScored(dice);
    return 1500;
  }

  if (is_two_triplets()) {
    MarkAllDiceScored(dice);
    return 2500;
  }

  // Process counts for standard scoring
  for (int i = 0; i < 6; ++i) {
    int count = counts[i];
    int face_value = i + 1;

    if (count >= 4) {
      // Score for four, five, or six of a kind
      if (count == 4) {
        MarkNumberAsScored(possibly_scoring_dice, face_value);
        score += 1000;
      } else if (count == 5) {
        MarkNumberAsScored(possibly_scoring_dice, face_value);
        score += 2000;
      } else if (count == 6) {
        MarkAllDiceScored(dice);
        score += 3000;
      }

      count = 0;  // We've scored these dice
      continue;
    }

    if (count == 3) {
      // Score for three of a kind
      score += (face_value == 1) ? 300 : face_value * 100;
      MarkNumberAsScored(possibly_scoring_dice, face_value);
      count = 0;  // We've scored these dice
      continue;
    }

    // Score for single dice
    if (face_value == 1) {
      score += count * 100;  // Each 1 is worth 100 points
      MarkNumberAsScored(possibly_scoring_dice, face_value);
    } else if (face_value == 5) {
      MarkNumberAsScored(possibly_scoring_dice, face_value);
      score += count * 50;  // Each 5 is worth 50 points
    }
  }

  return score;
}

class PlayerTurn {
 public:
 private:
  // The current score.
  int score_ = 0;

  // These are the dice.
  Dice dice_{};
};

class Strategy {
 public:
  // Returns true if we should keep going.
  virtual bool KeepGoing(const Dice& dice, int current_score) = 0;
};

class BasicStrategy : public Strategy {
  // Simplest strategy - keep going unless we only have 1 live die.
  bool KeepGoing(const Dice& dice, int current_score) override {
    return NumLiveDice(dice) > 1;
  }
};

int main() { return 0; }
