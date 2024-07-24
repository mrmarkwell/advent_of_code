/*
--- Day 15: Science for Hungry People ---
Today, you set out on the task of perfecting your milk-dunking cookie recipe.
All you have to do is find the right balance of ingredients.

Your recipe leaves room for exactly 100 teaspoons of ingredients. You make a
list of the remaining ingredients you could use to finish the recipe (your
puzzle input) and their properties per teaspoon:

capacity (how well it helps the cookie absorb milk)
durability (how well it keeps the cookie intact when full of milk)
flavor (how tasty it makes the cookie)
texture (how it improves the feel of the cookie)
calories (how many calories it adds to the cookie)
You can only measure ingredients in whole-teaspoon amounts accurately, and you
have to be accurate so you can reproduce your results in the future. The total
score of a cookie can be found by adding up each of the properties (negative
totals become 0) and then multiplying together everything except calories.

For instance, suppose you have these two ingredients:

Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories 8
Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3
Then, choosing to use 44 teaspoons of butterscotch and 56 teaspoons of cinnamon
(because the amounts of each ingredient must add up to 100) would result in a
cookie with the following properties:

A capacity of 44*-1 + 56*2 = 68
A durability of 44*-2 + 56*3 = 80
A flavor of 44*6 + 56*-2 = 152
A texture of 44*3 + 56*-1 = 76
Multiplying these together (68 * 80 * 152 * 76, ignoring calories for now)
results in a total score of 62842880, which happens to be the best score
possible given these ingredients. If any properties had produced a negative
total, it would have instead become zero, causing the whole score to multiply to
zero.

Given the ingredients in your kitchen and their properties, what is the total
score of the highest-scoring cookie you can make?

Your puzzle answer was 13882464.

The first half of this puzzle is complete! It provides one gold star: *

--- Part Two ---
Your cookie recipe becomes wildly popular! Someone asks if you can make another
recipe that has exactly 500 calories per cookie (so they can use it as a meal
replacement). Keep the rest of your award-winning process the same (100
teaspoons, same ingredients, same scoring system).

For example, given the ingredients above, if you had instead selected 40
teaspoons of butterscotch and 60 teaspoons of cinnamon (which still adds to
100), the total calorie count would be 40*8 + 60*3 = 500. The total score would
go down, though: only 57600000, the best you can do in such trying
circumstances.

Given the ingredients in your kitchen and their properties, what is the total
score of the highest-scoring cookie you can make with a calorie total of 500?
*/

#include <fmt/core.h>

#include <cassert>
#include <regex>
#include <vector>

#include "utils/utils.h"

struct Ingredient {
  std::string name;
  int capacity = 0;
  int durability = 0;
  int flavor = 0;
  int texture = 0;
  int calories = 0;
};

struct IngredientAmount {
  Ingredient ingredient;
  int tsp;
};

std::vector<IngredientAmount> GetIngredients(std::vector<std::string> lines) {
  constexpr int kNameIdx = 1;
  constexpr int kCapacityIdx = 2;
  constexpr int kDurabilityIdx = 3;
  constexpr int kFlavorIdx = 4;
  constexpr int kTextureIdx = 5;
  constexpr int kCalories = 6;

  // All the names we have seen so far.
  std::vector<IngredientAmount> recipe;
  std::regex pattern(
      R"((\w+): capacity (-?\d+), durability (-?\d+), flavor (-?\d+), texture (-?\d+), calories (-?\d+))");

  for (const auto& line : lines) {
    std::smatch match;
    std::regex_search(line, match, pattern);

    recipe.push_back(
        {.ingredient =
             Ingredient{.name = match[kNameIdx],
                        .capacity = std::stoi(match[kCapacityIdx]),
                        .durability = std::stoi(match[kDurabilityIdx]),
                        .flavor = std::stoi(match[kFlavorIdx]),
                        .texture = std::stoi(match[kTextureIdx]),
                        .calories = std::stoi(match[kCalories])},
         .tsp = 1});
  }
  return recipe;
}

int CalculateScore(std::vector<IngredientAmount> recipe,
                   bool check_calories = false) {
  int capacity = 0;
  int durability = 0;
  int flavor = 0;
  int texture = 0;
  int calories = 0;

  for (IngredientAmount ingredient_amount : recipe) {
    Ingredient ingredient = ingredient_amount.ingredient;
    int num_of_tsps = ingredient_amount.tsp;

    capacity += ingredient.capacity * num_of_tsps;
    durability += ingredient.durability * num_of_tsps;
    flavor += ingredient.flavor * num_of_tsps;
    texture += ingredient.texture * num_of_tsps;
    calories += ingredient.calories * num_of_tsps;
  }
  // Don't allow negative values.
  capacity = std::max(0, capacity);
  durability = std::max(0, durability);
  flavor = std::max(0, flavor);
  texture = std::max(0, texture);

  int score = capacity * durability * flavor * texture;

  if (check_calories == true && calories != 500) {
    return 0;
  }

  return score;
}

void TestCalculateScore() {
  std::vector<std::string> test_lines = {
      "Butterscotch: capacity -1, durability -2, flavor 6, texture 3, calories "
      "8",
      "Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories 3"};

  std::vector<IngredientAmount> test_recipe = GetIngredients(test_lines);
  test_recipe[0].tsp = 44;
  test_recipe[1].tsp = 56;

  fmt::print("Test recipe score: {}\n", CalculateScore(test_recipe));
  assert(CalculateScore(test_recipe) == 62842880);
}

void MakeThePerfectMilkDunkingCookieRecipe(
    std::vector<IngredientAmount>& recipe) {
  int num_of_tsps = recipe.size();
  while (num_of_tsps != 100) {
    IngredientAmount* next_ingredient_to_increase = nullptr;
    int best_score = 0;

    for (IngredientAmount& ingredient_amount : recipe) {
      int& tsps = ingredient_amount.tsp;
      tsps++;
      int current_score = CalculateScore(recipe);
      if (best_score < current_score) {
        next_ingredient_to_increase = &ingredient_amount;
        best_score = current_score;
      }
      tsps--;
    }

    next_ingredient_to_increase->tsp++;
    num_of_tsps++;
  }
}

int GetTheNastyHealthyMilkDunkingCookieRecipeScore(
    std::vector<IngredientAmount>& recipe) {
  int best_score = 0;
  for (int i = 0; i <= 100; i++) {
    for (int j = 0; j <= 100; j++) {
      for (int k = 0; k <= 100; k++) {
        int l = 100 - i - j - k;
        if (l < 0) {
          continue;
        }
        recipe[0].tsp = i;
        recipe[1].tsp = j;
        recipe[2].tsp = k;
        recipe[3].tsp = l;

        int old_score = best_score;
        best_score = std::max(best_score, CalculateScore(recipe, true));
        if (best_score != old_score) {
          fmt::print("Found a new best score {}: {}, {}, {}, {}\n", best_score,
                     i, j, k, l);
        }
      }
    }
  }
  return best_score;
}

void TestMakeThePerfectMilkDunkingCookieRecipe() {
  std::vector<std::string> test_lines = {
      "Butterscotch: capacity -1, durability -2, flavor 6, texture 3, "
      "calories "
      "8",
      "Cinnamon: capacity 2, durability 3, flavor -2, texture -1, calories "
      "3"};

  std::vector<IngredientAmount> test_recipe = GetIngredients(test_lines);

  for (auto ingredient_amount : test_recipe) {
    fmt::print("{}\n", ingredient_amount.ingredient.name);
  }
  MakeThePerfectMilkDunkingCookieRecipe(test_recipe);

  for (auto ingredient_amount : test_recipe) {
    fmt::print("{}\n", ingredient_amount.ingredient.name);
  }

  fmt::print("We need {} tsps of {} and {} tsps of {}\n", test_recipe[0].tsp,
             test_recipe[0].ingredient.name, test_recipe[1].tsp,
             test_recipe[1].ingredient.name);
  assert(test_recipe[0].tsp == 44 && test_recipe[1].tsp == 56);
}

void TheTest() {
  TestMakeThePerfectMilkDunkingCookieRecipe();
  TestCalculateScore();
}

int main() {
  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day15.txt");
  std::vector<IngredientAmount> recipe = GetIngredients(lines);

  // TheTest();

  MakeThePerfectMilkDunkingCookieRecipe(recipe);
  int score = CalculateScore(recipe);

  fmt::print("The score for the perfect milk dunking cookie recipe is {}\n",
             score);

  int new_score = GetTheNastyHealthyMilkDunkingCookieRecipeScore(recipe);

  fmt::print("The new healthy nasty milk dunking cookie recipe score is {}\n",
             new_score);

  return 0;
}
