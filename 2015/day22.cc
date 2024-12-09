/*
--- Day 22: Wizard Simulator 20XX ---
Little Henry Case decides that defeating bosses with swords and stuff is boring.
Now he's playing the game with a wizard. Of course, he gets stuck on another
boss and needs your help again.

In this version, combat still proceeds with the player and the boss taking
alternating turns. The player still goes first. Now, however, you don't get any
equipment; instead, you must choose one of your spells to cast. The first
character at or below 0 hit points loses.

Since you're a wizard, you don't get to wear armor, and you can't attack
normally. However, since you do magic damage, your opponent's armor is ignored,
and so the boss effectively has zero armor as well. As before, if armor (from a
spell, in this case) would reduce damage below 1, it becomes 1 instead - that
is, the boss' attacks always deal at least 1 damage.

On each of your turns, you must select one of your spells to cast. If you cannot
afford to cast any spell, you lose. Spells cost mana; you start with 500 mana,
but have no maximum limit. You must have enough mana to cast a spell, and its
cost is immediately deducted when you cast it. Your spells are Magic Missile,
Drain, Shield, Poison, and Recharge.

Magic Missile costs 53 mana. It instantly does 4 damage.
Drain costs 73 mana. It instantly does 2 damage and heals you for 2 hit points.
Shield costs 113 mana. It starts an effect that lasts for 6 turns. While it is
active, your armor is increased by 7. Poison costs 173 mana. It starts an effect
that lasts for 6 turns. At the start of each turn while it is active, it deals
the boss 3 damage. Recharge costs 229 mana. It starts an effect that lasts for 5
turns. At the start of each turn while it is active, it gives you 101 new mana.
Effects all work the same way. Effects apply at the start of both the player's
turns and the boss' turns. Effects are created with a timer (the number of turns
they last); at the start of each turn, after they apply any effect they have,
their timer is decreased by one. If this decreases the timer to zero, the effect
ends. You cannot cast a spell that would start an effect which is already
active. However, effects can be started on the same turn they end.

For example, suppose the player has 10 hit points and 250 mana, and that the
boss has 13 hit points and 8 damage:

-- Player turn --
- Player has 10 hit points, 0 armor, 250 mana
- Boss has 13 hit points
Player casts Poison.

-- Boss turn --
- Player has 10 hit points, 0 armor, 77 mana
- Boss has 13 hit points
Poison deals 3 damage; its timer is now 5.
Boss attacks for 8 damage.

-- Player turn --
- Player has 2 hit points, 0 armor, 77 mana
- Boss has 10 hit points
Poison deals 3 damage; its timer is now 4.
Player casts Magic Missile, dealing 4 damage.

-- Boss turn --
- Player has 2 hit points, 0 armor, 24 mana
- Boss has 3 hit points
Poison deals 3 damage. This kills the boss, and the player wins.
Now, suppose the same initial conditions, except that the boss has 14 hit points
instead:

-- Player turn --
- Player has 10 hit points, 0 armor, 250 mana
- Boss has 14 hit points
Player casts Recharge.

-- Boss turn --
- Player has 10 hit points, 0 armor, 21 mana
- Boss has 14 hit points
Recharge provides 101 mana; its timer is now 4.
Boss attacks for 8 damage!

-- Player turn --
- Player has 2 hit points, 0 armor, 122 mana
- Boss has 14 hit points
Recharge provides 101 mana; its timer is now 3.
Player casts Shield, increasing armor by 7.

-- Boss turn --
- Player has 2 hit points, 7 armor, 110 mana
- Boss has 14 hit points
Shield's timer is now 5.
Recharge provides 101 mana; its timer is now 2.
Boss attacks for 8 - 7 = 1 damage!

-- Player turn --
- Player has 1 hit point, 7 armor, 211 mana
- Boss has 14 hit points
Shield's timer is now 4.
Recharge provides 101 mana; its timer is now 1.
Player casts Drain, dealing 2 damage, and healing 2 hit points.

-- Boss turn --
- Player has 3 hit points, 7 armor, 239 mana
- Boss has 12 hit points
Shield's timer is now 3.
Recharge provides 101 mana; its timer is now 0.
Recharge wears off.
Boss attacks for 8 - 7 = 1 damage!

-- Player turn --
- Player has 2 hit points, 7 armor, 340 mana
- Boss has 12 hit points
Shield's timer is now 2.
Player casts Poison.

-- Boss turn --
- Player has 2 hit points, 7 armor, 167 mana
- Boss has 12 hit points
Shield's timer is now 1.
Poison deals 3 damage; its timer is now 5.
Boss attacks for 8 - 7 = 1 damage!

-- Player turn --
- Player has 1 hit point, 7 armor, 167 mana
- Boss has 9 hit points
Shield's timer is now 0.
Shield wears off, decreasing armor by 7.
Poison deals 3 damage; its timer is now 4.
Player casts Magic Missile, dealing 4 damage.

-- Boss turn --
- Player has 1 hit point, 0 armor, 114 mana
- Boss has 2 hit points
Poison deals 3 damage. This kills the boss, and the player wins.
You start with 50 hit points and 500 mana points. The boss's actual stats are in
your puzzle input. What is the least amount of mana you can spend and still win
the fight? (Do not include mana recharge effects as "spending" negative mana.)
*/
#include <print>

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

struct Character {
  int health = 0;
  int damage = 0;
  int armor = 0;
  int mana = 0;

  // Take a hit for 'incoming_damage' damage.
  void TakeHit(int incoming_damage) {
    int damage_received = std::max(1, incoming_damage - armor);
    health -= damage_received;
  }

  void Heal(int heal_amount) { health += heal_amount; }

  bool IsDead() { return health <= 0; }
};

enum class SpellType {
  kMagicMissile,
  kDrain,
  kShield,
  kPoison,
  kRecharge,
};
// Function to convert SpellType to string
std::string ToString(SpellType spell_type) {
  switch (spell_type) {
    case SpellType::kMagicMissile:
      return "Magic Missile";
    case SpellType::kDrain:
      return "Drain";
    case SpellType::kShield:
      return "Shield";
    case SpellType::kPoison:
      return "Poison";
    case SpellType::kRecharge:
      return "Recharge";
    default:
      return "Unknown";
  }
}
// Function to pretty-print a list of SpellType values
void PrettyPrintSpellList(const std::vector<SpellType>& spells) {
  bool first = true;
  for (const auto& spell : spells) {
    if (!first) {
      std::print(", ");
    }
    std::print("{}", ToString(spell));
    first = false;
  }
  std::print("\n");
}
struct Spell {
  int turns_remaining = 0;
  int mana_cost = 0;
  SpellType type;
};

void ApplySpell(Spell& spell, Character& wizard, Character& boss) {
  spell.turns_remaining--;
  switch (spell.type) {
    case SpellType::kMagicMissile: {
      boss.TakeHit(4);
      break;
    }
    case SpellType::kDrain: {
      boss.TakeHit(2);
      wizard.Heal(2);
      break;
    }
    case SpellType::kShield: {
      wizard.armor = 7;
      if (spell.turns_remaining == 0) {
        // Reset.
        wizard.armor = 0;
      }
      break;
    }
    case SpellType::kPoison: {
      boss.TakeHit(3);
      break;
    }
    case SpellType::kRecharge: {
      wizard.mana += 101;
      break;
    }
  }
}

struct GameState {
  int mana_spent = 0;
  Character wizard{.health = 50, .mana = 500};
  Character boss{.health = 55, .damage = 8, .armor = 0};
  std::vector<Spell> active_spells;
  std::vector<SpellType> spells_cast;

  void ApplyActiveSpells() {
    for (auto& spell : active_spells) {
      ApplySpell(spell, wizard, boss);
    }
    RemoveExpiredSpells();
  }

  std::vector<Spell> GetPossibleSpells() {
    std::vector<Spell> possible_spells = {
        {.turns_remaining = 1,
         .mana_cost = 53,
         .type = SpellType::kMagicMissile},
        {.turns_remaining = 1, .mana_cost = 73, .type = SpellType::kDrain},
        {.turns_remaining = 6, .mana_cost = 113, .type = SpellType::kShield},
        {.turns_remaining = 6, .mana_cost = 173, .type = SpellType::kPoison},
        {.turns_remaining = 5, .mana_cost = 229, .type = SpellType::kRecharge}};

    // Remove all spells that are too expensive, or are already active.
    possible_spells.erase(
        std::remove_if(possible_spells.begin(), possible_spells.end(),
                       [this](const auto& spell) {
                         if (spell.mana_cost > wizard.mana) {
                           return true;
                         }
                         for (const auto& active_spell : active_spells) {
                           if (active_spell.type == spell.type) {
                             return true;
                           }
                         }
                         return false;
                       }),
        possible_spells.end());
    return possible_spells;
  }

  void CastSpell(Spell spell) {
    mana_spent += spell.mana_cost;
    wizard.mana -= spell.mana_cost;
    if (wizard.mana < 0) {
      std::print("Error!! Spell: {}\n", ToString(spell.type));
      std::exit(0);
    }
    // std::print("Cast spell {}. Remaining mana: {}\n", ToString(spell.type),
    //            wizard.mana);
    spells_cast.push_back(spell.type);
    // Some spells are immediate.
    if (spell.type == SpellType::kMagicMissile ||
        spell.type == SpellType::kDrain) {
      ApplySpell(spell, wizard, boss);
      return;
    }
    // All other spells don't take effect immediately.
    active_spells.push_back(spell);
  }

  void BossTurn() { wizard.TakeHit(boss.damage); }
  bool WizardWon() { return boss.IsDead(); }
  bool BossWon() { return wizard.IsDead(); }

  bool GameOver(int& min_mana_spent) {
    if (WizardWon()) {
      int old_mana_spent = min_mana_spent;
      min_mana_spent = std::min(min_mana_spent, mana_spent);
      if (min_mana_spent != old_mana_spent) {
        std::print("Found a new best strategy! Mana spent: {}\n",
                   min_mana_spent);
        PrettyPrintSpellList(spells_cast);
      }
      return true;
    }
    if (BossWon()) {
      return true;
    }
    return false;
  }

 private:
  void RemoveExpiredSpells() {
    active_spells.erase(
        std::remove_if(
            active_spells.begin(), active_spells.end(),
            [](const Spell& spell) { return spell.turns_remaining <= 0; }),
        active_spells.end());
  }
};

void TakeGameTurn(GameState state, int& min_mana_spent,
                  bool hard_mode = false) {
  if (hard_mode) {
    state.wizard.health--;
  }
  if (state.GameOver(min_mana_spent)) return;
  if (state.mana_spent >= min_mana_spent) return;
  state.ApplyActiveSpells();
  if (state.GameOver(min_mana_spent)) return;
  // The next game states possible are all the possible spells we could cast.
  // Iterate through them, casting them and recursing.
  for (auto& spell : state.GetPossibleSpells()) {
    GameState next_state = state;
    next_state.CastSpell(spell);
    if (next_state.GameOver(min_mana_spent)) return;
    // Now it is the boss's turn. The active spells are applied and we check if
    // the game is over before and after the boss makes his attack.
    next_state.ApplyActiveSpells();
    if (next_state.GameOver(min_mana_spent)) return;
    next_state.BossTurn();
    if (next_state.GameOver(min_mana_spent)) return;
    TakeGameTurn(std::move(next_state), min_mana_spent, hard_mode);
  }
  return;
}

int main() {
  int min_mana_spent = std::numeric_limits<int>::max();
  TakeGameTurn(GameState{}, min_mana_spent);
  int hard_mode_min_mana_spent = std::numeric_limits<int>::max();
  TakeGameTurn(GameState{}, hard_mode_min_mana_spent, /*hard_mode=*/true);
  std::print("Part 1: {} Part 2: {}\n", min_mana_spent,
             hard_mode_min_mana_spent);
}
