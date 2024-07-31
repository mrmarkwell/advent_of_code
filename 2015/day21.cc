/*
--- Day 21: RPG Simulator 20XX ---
Little Henry Case got a new video game for Christmas. It's an RPG, and he's
stuck on a boss. He needs to know what equipment to buy at the shop. He hands
you the controller.

In this game, the player (you) and the enemy (the boss) take turns attacking.
The player always goes first. Each attack reduces the opponent's hit points by
at least 1. The first character at or below 0 hit points loses.

Damage dealt by an attacker each turn is equal to the attacker's damage score
minus the defender's armor score. An attacker always does at least 1 damage. So,
if the attacker has a damage score of 8, and the defender has an armor score of
3, the defender loses 5 hit points. If the defender had an armor score of 300,
the defender would still lose 1 hit point.

Your damage score and armor score both start at zero. They can be increased by
buying items in exchange for gold. You start with no items and have as much gold
as you need. Your total damage or armor is equal to the sum of those stats from
all of your items. You have 100 hit points.

Here is what the item shop is selling:

Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3
You must buy exactly one weapon; no dual-wielding. Armor is optional, but you
can't use more than one. You can buy 0-2 rings (at most one for each hand). You
must use any items you buy. The shop only has one of each item, so you can't
buy, for example, two rings of Damage +3.

For example, suppose you have 8 hit points, 5 damage, and 5 armor, and that the
boss has 12 hit points, 7 damage, and 2 armor:

The player deals 5-2 = 3 damage; the boss goes down to 9 hit points.
The boss deals 7-5 = 2 damage; the player goes down to 6 hit points.
The player deals 5-2 = 3 damage; the boss goes down to 6 hit points.
The boss deals 7-5 = 2 damage; the player goes down to 4 hit points.
The player deals 5-2 = 3 damage; the boss goes down to 3 hit points.
The boss deals 7-5 = 2 damage; the player goes down to 2 hit points.
The player deals 5-2 = 3 damage; the boss goes down to 0 hit points.
In this scenario, the player wins! (Barely.)

You have 100 hit points. The boss's actual stats are in your puzzle input. What
is the least amount of gold you can spend and still win the fight?
*/
#include <fmt/core.h>

#include <algorithm>
#include <optional>
#include <vector>

struct Character {
  int health = 0;
  int damage = 0;
  int armor = 0;

  // Take a hit for 'incoming_damage' damage.
  void TakeHit(int incoming_damage) {
    int damage_received = std::max(1, incoming_damage - armor);
    health -= damage_received;
  }

  bool IsDead() { return health <= 0; }
};

enum class Type {
  kWeapon,
  kArmor,
  kRing,
};

struct Item {
  std::string name;
  int cost = 0;
  int damage = 0;
  int armor = 0;
  Type type;
};

struct Player : public Character {
  bool IsValid() { return weapon.has_value(); }
  std::optional<Item> weapon = std::nullopt;
  std::optional<Item> armor_set = std::nullopt;
  std::optional<Item> right_ring = std::nullopt;
  std::optional<Item> left_ring = std::nullopt;

  void Print() {
    fmt::print("Equipment:[{}, {}, {}, {}] Cost: {} Win: {}\n",
               weapon ? weapon->name : "None",
               armor_set ? armor_set->name : "None",
               right_ring ? right_ring->name : "None",
               left_ring ? left_ring->name : "None", Cost(),
               WouldWin() ? "Yes" : "No");
  }

  bool CanTake(const Item& item) {
    if (item.type == Type::kWeapon && !weapon) {
      return true;
    }
    if (item.type == Type::kArmor && !armor) {
      return true;
    }
    if (item.type == Type::kRing && (!right_ring || !left_ring)) {
      return true;
    }
    return false;
  }

  void Take(const Item& item) {
    switch (item.type) {
      case Type::kWeapon: {
        weapon = item;
        damage += item.damage;
        break;
      }
      case Type::kArmor: {
        armor_set = item;
        armor += item.armor;
        break;
      }
      case Type::kRing: {
        armor += item.armor;
        damage += item.damage;
        if (right_ring) {
          left_ring = item;
          break;
        }
        right_ring = item;
        break;
      }
    }
  }
  int Cost() {
    int cost = 0;
    cost += weapon ? weapon->cost : 0;
    cost += armor_set ? armor_set->cost : 0;
    cost += right_ring ? right_ring->cost : 0;
    cost += left_ring ? left_ring->cost : 0;
    if (cost < 0) {
      fmt::print("Cost < 0. Weapon: {}: {}, {}: {}, {}: {}, {}: {}\n",
                 weapon ? weapon->name : "None", weapon ? weapon->cost : -1,
                 armor_set ? armor_set->name : "None",
                 armor_set ? armor_set->cost : -1,
                 right_ring ? right_ring->name : "None",
                 right_ring ? right_ring->cost : -1,
                 left_ring ? left_ring->name : "None",
                 left_ring ? left_ring->cost : -1);
    }
    return cost;
  }

  bool WouldWin() {
    Character boss{.health = 100, .damage = 8, .armor = 2};

    // Reset player health before attempting any battle.
    health = 100;
    while (true) {
      boss.TakeHit(damage);
      if (boss.IsDead()) return true;
      TakeHit(boss.damage);
      if (IsDead()) return false;
    }
    return false;
  }
  bool IsFull() { return weapon && armor && right_ring && left_ring; }
};

void FindCheapestWinningPlayer(const std::vector<Item>& shop, Player player,
                               int shop_idx, int& cheapest_win) {
  if (player.IsFull() || shop.size() <= shop_idx) {
    // We've exhausted the shop, or the player can't take any more items.
    return;
  }
  Item next_item = shop[shop_idx];

  if (player.CanTake(next_item)) {
    // Recurse in two directions - with and without this item.
    Player new_player = player;
    new_player.Take(next_item);
    new_player.Print();
    if (new_player.IsValid() && new_player.WouldWin()) {
      // This is the cheapest winning player!

      cheapest_win = std::min(new_player.Cost(), cheapest_win);
      // fmt::print("Found a cheap win! Cost: {}, Cheapest: {}\n",
      //            player.Cost(), cheapest_win);
    }
    FindCheapestWinningPlayer(shop, new_player, shop_idx + 1, cheapest_win);
  }
  // Recurse with the next shop_idx.
  FindCheapestWinningPlayer(shop, player, shop_idx + 1, cheapest_win);
  return;
}

int main() {
  /*

Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3

  */
  // Initialize the vector with items
  std::vector<Item> shop = {{"Dagger", 8, 4, 0, Type::kWeapon},
                            {"Shortsword", 10, 5, 0, Type::kWeapon},
                            {"Warhammer", 25, 6, 0, Type::kWeapon},
                            {"Longsword", 40, 7, 0, Type::kWeapon},
                            {"Greataxe", 74, 8, 0, Type::kWeapon},
                            {"Leather", 13, 0, 1, Type::kArmor},
                            {"Chainmail", 31, 0, 2, Type::kArmor},
                            {"Splintmail", 53, 0, 3, Type::kArmor},
                            {"Bandedmail", 75, 0, 4, Type::kArmor},
                            {"Platemail", 102, 0, 5, Type::kArmor},
                            {"Damage1", 25, 1, 0, Type::kRing},
                            {"Damage2", 50, 2, 0, Type::kRing},
                            {"Damage3", 100, 3, 0, Type::kRing},
                            {"Armor1", 20, 0, 1, Type::kRing},
                            {"Armor2", 40, 0, 2, Type::kRing},
                            {"Armor3", 80, 0, 3, Type::kRing}};

  // Sorting the items based on cost
  std::sort(shop.begin(), shop.end(),
            [](const Item& a, const Item& b) { return a.cost < b.cost; });

  int cheapest_win = std::numeric_limits<int>::max();
  FindCheapestWinningPlayer(shop, Player{}, /*shop_idx=*/0, cheapest_win);

  fmt::print("Cheapest_win: {}\n", cheapest_win);

  return 0;
}
