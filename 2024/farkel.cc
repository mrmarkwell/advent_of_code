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

  void SetValue(int value) { value_ = value; }
  int Value() const { return value_; }
  bool IsLive() const { return state_ == State::kLive; }
  bool IsScored() const { return state_ == State::kScored; }

  // if the dice are hot, they need to be made live again and marked as not
  // scored.
  void Reset() { state_ = State::kLive; }
  void MarkScored() { state_ = State::kScored; }

  // Comparison operator for sorting
  bool operator<(const Die& other) const {
    // Sort by values after categorizing by states.
    if (state_ != other.state_) {
      return state_ < other.state_;
    }
    return value_ < other.value_;
  }

 private:
  int value_ = 1;

  // Current state of the die.
  State state_ = State::kLive;
};

// alias to represent all the dice.
using Dice = std::array<Die, kNumDice>;

//*** Helper functions that operate on dice. ***//

void RollDice(Dice& dice) {
  std::for_each(dice.begin(), dice.end(), [](Die& die) { die.Roll(); });
}

void SortDice(Dice& dice) { std::sort(dice.begin(), dice.end()); }

// Returns the number of live dice.
int NumLiveDice(const Dice& dice) {
  return std::count_if(dice.cbegin(), dice.cend(),
                       [](const Die& die) { return die.IsLive(); });
}

void ResetDiceIfHot(Dice& dice) {
  if (NumLiveDice(dice) == 0) {
    // std::print("Hot Dice! Hell yeah!\n");
    std::for_each(dice.begin(), dice.end(), [](Die& die) { die.Reset(); });
  }
}

// Make all dice live again.
void ResetHotDice(Dice& dice) {}

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
    if (values.empty()) {
      std::print(" ");
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
    return three_pairs || four_and_two;
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

class Strategy {
 public:
  // Returns true if we should keep going.
  virtual bool KeepGoing(const Dice& dice, int current_score) const = 0;
  virtual std::string GetName() const = 0;
  virtual ~Strategy() {};
};

class KeepGoingTill : public Strategy {
 public:
  KeepGoingTill(int target) : target_(target) {}
  // Simplest strategy - keep going unless we only have 1 live die.
  bool KeepGoing(const Dice& dice, int current_score) const override {
    return current_score < target_;
  }
  std::string GetName() const override {
    return std::format("KeepGoingTill{}", target_);
  }

 private:
  int target_ = 0;
};

class KeepGoingWithXOrMoreDice : public Strategy {
 public:
  KeepGoingWithXOrMoreDice(int x) : x_(x) {}
  // Simplest strategy - keep going unless we only have 1 live die.
  bool KeepGoing(const Dice& dice, int current_score) const override {
    return NumLiveDice(dice) >= x_;
  }
  std::string GetName() const override {
    return std::format("KeepGoingWith{}OrMore", x_);
  }

 private:
  int x_ = 0;
};

class CombineStrategies : public Strategy {
 public:
  CombineStrategies(std::unique_ptr<Strategy> strategy_one,
                    std::unique_ptr<Strategy> strategy_two)
      : s1_(std::move(strategy_one)), s2_(std::move(strategy_two)) {}
  bool KeepGoing(const Dice& dice, int current_score) const override {
    return s1_->KeepGoing(dice, current_score) ||
           s2_->KeepGoing(dice, current_score);
  }
  std::string GetName() const override {
    return std::format("{} OR {}", s1_->GetName(), s2_->GetName());
  }

 private:
  std::unique_ptr<Strategy> s1_;
  std::unique_ptr<Strategy> s2_;
};

class CustomRiskProfile : public Strategy {
 public:
  CustomRiskProfile(int first, int second, int third, int fourth)
      : first_(first), second_(second), third_(third), fourth_(fourth) {}

  bool KeepGoing(const Dice& dice, int current_score) const override {
    if (current_score > fourth_) {
      return NumLiveDice(dice) >= 5;
    }
    if (current_score > third_) {
      return NumLiveDice(dice) >= 4;
    }
    if (current_score > second_) {
      // Roll if you have at least 3.
      // Stop if you have only 2.
      return NumLiveDice(dice) >= 3;
    }
    if (current_score > first_) {
      // Roll if you have at least 2.
      // Stop if you have only 1.
      return NumLiveDice(dice) >= 2;
    }
    return true;
  }

  std::string GetName() const override {
    return std::format("CustomRiskProfile{}x{}x{}x{}", first_, second_, third_,
                       fourth_);
  }

 private:
  int first_;
  int second_;
  int third_;
  int fourth_;
};

class RiskProfile : public Strategy {
 public:
  RiskProfile(int start, int step) : start_(start), step_(step) {}

  bool KeepGoing(const Dice& dice, int current_score) const override {
    if (current_score > (start_ + 3 * step_)) {
      return NumLiveDice(dice) >= 5;
    }
    if (current_score > (start_ + 2 * step_)) {
      return NumLiveDice(dice) >= 4;
    }
    if (current_score > (start_ + step_)) {
      return NumLiveDice(dice) >= 3;
    }
    if (current_score > start_) {
      return NumLiveDice(dice) >= 2;
    }
    return true;
  }

  std::string GetName() const override {
    return std::format("RiskProfile{}x{}", start_, step_);
  }

 private:
  int start_;
  int step_;
};

int TakeTurn(const Strategy& strategy, Dice dice = Dice{}) {
  int turn_score = 0;
  while (true) {
    RollDice(dice);
    SortDice(dice);
    // PrintDice(dice);
    int current_score = Score(dice);
    turn_score += current_score;
    // std::print(" Roll Score: {} Turn Score: {}\n", current_score,
    // turn_score);
    if (current_score == 0) {
      // std::print("Farkel'd with turn score {}!\n", turn_score);
      return 0;
    }
    ResetDiceIfHot(dice);
    if (!strategy.KeepGoing(dice, turn_score)) {
      // std::print("Player decides to stop with score {}. Final Dice: ",
      // turn_score); PrintDice(dice); std::print("\n");
      return turn_score;
    }
  }
}

void PrintFractionsAndPercentages(
    const std::vector<std::unique_ptr<Strategy>>& strategies,
    const std::vector<int>& wins) {
  struct Player {
    Strategy* strategy;
    int wins;

    bool operator<(const Player& other) const { return wins < other.wins; }
  };

  std::vector<Player> players;
  for (int i = 0; i < strategies.size(); ++i) {
    players.push_back({.strategy = strategies[i].get(), .wins = wins[i]});
  }

  std::sort(players.begin(), players.end());
  std::reverse(players.begin(), players.end());

  // Compute the total sum of the vector elements.
  int total = std::accumulate(wins.begin(), wins.end(), 0);

  if (total == 0) {
    std::print(
        "The total sum of the elements is zero. Cannot compute fractions or "
        "percentages.\n");
    return;
  }

  // Print the fraction and percentage for each element.
  constexpr int kStrategyNameWidth = 45;
  std::print("{:<{}} Wins     Percentage\n", "Strategy", kStrategyNameWidth);

  for (int i = 0; i < players.size(); ++i) {
    int value = players[i].wins;
    double fraction = static_cast<double>(value) / total;
    double percentage = fraction * 100;
    std::print("{:<{}} {:<8d} {:<10.2f}%\n", players[i].strategy->GetName(),
               kStrategyNameWidth, value, percentage);
  }
}

class Game {
 public:
  Game(std::vector<std::unique_ptr<Strategy>> players)
      : players_(std::move(players)),
        scores_(players_.size(), 0),
        num_wins_(players_.size(), 0) {}

  void SimulateGames(int target_score, int num_games) {
    for (int i = 0; i < num_games; ++i) {
      PlayGame(target_score);
    }
    PrintFractionsAndPercentages(players_, num_wins_);
  }

 private:
  void PlayGame(int target_score) {
    ClearScores();
    bool we_have_a_winner = false;

    while (!we_have_a_winner) {
      for (int player = 0; player < players_.size(); ++player) {
        scores_[player] += TakeTurn(*players_[player]);
        if (scores_[player] > target_score) {
          we_have_a_winner = true;
        }
      }
    }
    int winning_player = std::distance(
        scores_.begin(), std::max_element(scores_.begin(), scores_.end()));
    num_wins_[winning_player]++;
  }

  void ClearScores() { std::fill(scores_.begin(), scores_.end(), 0); }

  std::vector<std::unique_ptr<Strategy>> players_;
  std::vector<int> scores_{};
  std::vector<int> num_wins_{};
};

// Keep going till only 1 die: 370 mean, 550 std dev, 47% farkel.
// Keep going till only 2 die: 480 mean, 510 std dev, 20% farkel.

void TestSingleStrategy() {
  constexpr int kNumIterations = 30000;
  // constexpr BasicStrategy basic_strategy{};
  // constexpr StopWithTwo strategy{};
  // MoreConservativeWithHigherScore strategy{};
  // KeepGoingTill strategy{500};
  for (int target_score : {100, 200, 300, 400, 500, 600, 700, 800, 900}) {
    CombineStrategies strategy(std::make_unique<KeepGoingTill>(target_score),
                               std::make_unique<KeepGoingWithXOrMoreDice>(4));

    double mean = 0.0;
    double m2 = 0.0;  // Sum of squares of differences from the current mean.
    int count = 0;
    int num_farkels = 0;
    int max_score = 0;

    // Welford's algorithm for calculating mean and variance in a single pass.
    for (int i = 0; i < kNumIterations; ++i) {
      int value = TakeTurn(strategy);
      max_score = std::max(max_score, value);
      // std::print("======== Turn {}: {} =========\n", i + 1, value);
      if (value == 0) ++num_farkels;
      ++count;
      double delta = value - mean;
      mean += delta / count;
      double delta2 = value - mean;
      m2 += delta * delta2;
    }

    double variance = m2 / count;
    double std_dev = std::sqrt(variance);
    double farkel_percentage = (num_farkels * 100.0) / kNumIterations;

    // Print the results.
    std::print("Statistics for target {}:", target_score);
    std::print(" Mean: {:.2f}", mean);
    std::print(" Standard Deviation: {:.2f}", std_dev);
    std::print(" Max: {}", max_score);
    std::print(" Farkel Percentage: {:.2f}\n", farkel_percentage);
  }
}

int main() {
  constexpr int kTargetScore = 10000;
  constexpr int kNumGames = 100000;
  std::vector<std::unique_ptr<Strategy>> players;
  // players.push_back(std::make_unique<CombineStrategies>(
  //     std::make_unique<KeepGoingTill>(800),
  //     std::make_unique<KeepGoingWithXOrMoreDice>(3)));
  //
  // for (int i = 3; i <= 5; ++i) {
  //   // players.push_back(std::make_unique<KeepGoingWithXOrMoreDice>(i));
  //   for (int target_score : {400, 450}) {
  //     players.push_back(std::make_unique<CombineStrategies>(
  //         std::make_unique<KeepGoingTill>(target_score),
  //         std::make_unique<KeepGoingWithXOrMoreDice>(i)));
  //   }
  // }
  // for (int start : {200}) {
  //   for (int step : {200}) {
  //     players.push_back(std::make_unique<RiskProfile>(start, step));
  //   }
  // }
  // players.push_back(std::make_unique<CustomRiskProfile>(0, 300, 500, 1200));
  players.push_back(std::make_unique<CustomRiskProfile>(0, 200, 400, 800));
  // players.push_back(std::make_unique<CustomRiskProfile>(0, 200, 400, 1800));
  // players.push_back(std::make_unique<CustomRiskProfile>(0, 200, 350, 800));
  /// players.push_back(std::make_unique<CustomRiskProfile>(0, 200, 400, 800));
  // players.push_back(std::make_unique<CustomRiskProfile>(0, 0, 400, 800));
  // players.push_back(std::make_unique<CustomRiskProfile>(0, 0, 800, 1200));
  //  players.push_back(std::make_unique<CustomRiskProfile>(300, 500, 700,
  //  900));
  //   players.push_back(std::make_unique<CustomRiskProfile>(0, 200, 400, 800));
  //    players.push_back(std::make_unique<CustomRiskProfile>(0, 300, 900,
  //    1800));
  // players.push_back(std::make_unique<CustomRiskProfile>(500, 600, 700, 800));

  // players.push_back(std::make_unique<CustomRiskProfile>(100, 300, 500,
  // 1200));

  // players.push_back(std::make_unique<CustomRiskProfile>(100, 300, 500,
  // 1200));

  players.push_back(std::make_unique<CombineStrategies>(
      std::make_unique<KeepGoingTill>(400),
      std::make_unique<KeepGoingWithXOrMoreDice>(4)));
  //
  Game game(std::move(players));
  game.SimulateGames(kTargetScore, kNumGames);

  return 0;
}
