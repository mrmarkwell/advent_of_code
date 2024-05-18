/*
--- Day 7: Some Assembly Required ---
This year, Santa brought little Bobby Tables a set of wires and bitwise logic
gates! Unfortunately, little Bobby is a little under the recommended age range,
and he needs help assembling the circuit.

Each wire has an identifier (some lowercase letters) and can carry a 16-bit
signal (a number from 0 to 65535). A signal is provided to each wire by a gate,
another wire, or some specific value. Each wire can only get a signal from one
source, but can provide its signal to multiple destinations. A gate provides no
signal until all of its inputs have a signal.

The included instructions booklet describes how to connect the parts together: x
AND y -> z means to connect wires x and y to an AND gate, and then connect its
output to wire z.

For example:

123 -> x means that the signal 123 is provided to wire x.
x AND y -> z means that the bitwise AND of wire x and wire y is provided to wire
z. p LSHIFT 2 -> q means that the value from wire p is left-shifted by 2 and
then provided to wire q. NOT e -> f means that the bitwise complement of the
value from wire e is provided to wire f. Other possible gates include OR
(bitwise OR) and RSHIFT (right-shift). If, for some reason, you'd like to
emulate the circuit instead, almost all programming languages (for example, C,
JavaScript, or Python) provide operators for these gates.

For example, here is a simple circuit:

123 -> x
456 -> y
x AND y -> d
x OR y -> e
x LSHIFT 2 -> f
y RSHIFT 2 -> g
NOT x -> h
NOT y -> i
After it is run, these are the signals on the wires:

d: 72
e: 507
f: 492
g: 114
h: 65412
i: 65079
x: 123
y: 456
In little Bobby's kit's instructions booklet (provided as your puzzle input),
what signal is ultimately provided to wire a?

--- Part Two ---
Now, take the signal you got on wire a, override wire b to that signal, and
reset the other wires (including wire a). What new signal is ultimately provided
to wire a?
*/

#include <optional>
#include <regex>

#include "absl/container/flat_hash_map.h"
#include "utils/utils.h"

void PrettyPrintWires(const absl::flat_hash_map<std::string, uint16_t>& wires) {
  std::cout << "{\n";
  for (const auto& pair : wires) {
    std::cout << "  \"" << pair.first << "\": " << pair.second << ",\n";
  }
  std::cout << "}\n";
}

enum class Operation {
  kAnd,
  kOr,
  kNot,
  kLshift,
  kRshift,
  kAssignment,
};

class Operand {
 public:
  explicit Operand(std::string operand) {
    // If the operand is a number, save it as a literal. Otherwise save the
    // string directly.
    char* end = nullptr;
    uint16_t converted_val =
        static_cast<uint16_t>(std::strtoul(operand.c_str(), &end, 10));

    // If no characters were converted, this is a name.
    if (end == operand.c_str()) {
      wire = operand;
    } else {
      literal_value = converted_val;
    }
  }
  bool CanExecute(absl::flat_hash_map<std::string, uint16_t>& wires) const {
    // An operand is ready to be used if it is a wire with an assigned value, or
    // if it is a literal.
    return literal_value.has_value() || wires.contains(wire);
  }

  std::string ToString() const {
    if (literal_value) {
      return std::to_string(*literal_value);
    } else {
      return wire;
    }
  }

  uint16_t Value(absl::flat_hash_map<std::string, uint16_t>& wires) const {
    if (literal_value) {
      return *literal_value;
    } else {
      return wires[wire];
    }
  }

 private:
  // This is ether a literal integer or the name of a wire.
  std::optional<uint16_t> literal_value;
  std::string wire;
};

struct Instruction {
  Operation operation;
  std::vector<Operand> operands;
  std::string destination;
  bool executed = false;
  bool CanExecute(absl::flat_hash_map<std::string, uint16_t>& wires) const {
    // Can execute if it isn't already executed and all the operands are
    // available.
    return !executed && std::all_of(operands.begin(), operands.end(),
                                    [&wires](const auto& op) {
                                      return op.CanExecute(wires);
                                    });
  }
  void PrettyPrint() const {
    std::cout << "Operation: " << OperationToString(operation) << "\n";
    std::cout << "Operands: ";
    for (const auto& operand : operands) {
      std::cout << operand.ToString() << " ";
    }
    std::cout << "\nDestination: " << destination << "\n";
    std::cout << "Executed: " << (executed ? "true" : "false") << "\n";
  }

 private:
  static std::string OperationToString(Operation op) {
    switch (op) {
      case Operation::kAnd:
        return "AND";
      case Operation::kOr:
        return "OR";
      case Operation::kNot:
        return "NOT";
      case Operation::kLshift:
        return "LSHIFT";
      case Operation::kRshift:
        return "RSHIFT";
      case Operation::kAssignment:
        return "ASSIGNMENT";
      default:
        return "UNKNOWN";
    }
  }
};

// Executes if possible. Returns true if so.
void TryExecute(Instruction& inst,
                absl::flat_hash_map<std::string, uint16_t>& wires) {
  if (!inst.CanExecute(wires)) {
    return;
  }
  inst.executed = true;
  switch (inst.operation) {
    case Operation::kAssignment: {
      wires[inst.destination] = inst.operands[0].Value(wires);
      break;
    }
    case Operation::kAnd: {
      wires[inst.destination] =
          inst.operands[0].Value(wires) & inst.operands[1].Value(wires);
      break;
    }
    case Operation::kOr: {
      wires[inst.destination] =
          inst.operands[0].Value(wires) | inst.operands[1].Value(wires);
      break;
    }
    case Operation::kNot: {
      wires[inst.destination] = ~inst.operands[0].Value(wires);
      break;
    }
    case Operation::kLshift: {
      wires[inst.destination] = inst.operands[0].Value(wires)
                                << inst.operands[1].Value(wires);
      break;
    }
    case Operation::kRshift: {
      wires[inst.destination] =
          inst.operands[0].Value(wires) >> inst.operands[1].Value(wires);
      break;
    }
  }
}

Operation StringToOperation(const std::string& operation) {
  if (operation == "AND") {
    return Operation::kAnd;
  } else if (operation == "OR") {
    return Operation::kOr;
  } else if (operation == "NOT") {
    return Operation::kNot;
  } else if (operation == "LSHIFT") {
    return Operation::kLshift;
  } else if (operation == "RSHIFT") {
    return Operation::kRshift;
  } else {
    assert(false && "Unknown Op!");
  }
  return Operation::kOr;
}

std::vector<Instruction> ParseInstructions() {
  std::regex re(
      R"(((\w+)\s+(\w+)\s+(\w+)\s+->\s+(\w+))|(NOT\s+(\w+)\s+->\s+(\w+))|((\w+)\s+->\s+(\w+)))");
  constexpr int kIsBinary = 1;
  constexpr int kBinOp1 = 2;
  constexpr int kBinOperation = 3;
  constexpr int kBinOp2 = 4;
  constexpr int kBinDest = 5;
  constexpr int kIsNotOp = 6;
  constexpr int kNotOp = 7;
  constexpr int kNotDest = 8;
  constexpr int kIsAssignment = 9;
  constexpr int kAssignmentOp = 10;
  constexpr int kAssignmentDest = 11;

  std::vector<std::string> lines =
      aoc::LoadStringsFromFileByLine("./2015/day7alternate.txt");

  std::vector<Instruction> instructions;

  for (const std::string& line : lines) {
    std::smatch match;
    if (std::regex_match(line, match, re)) {
      if (match[kIsBinary].matched) {
        instructions.push_back(
            {.operation = StringToOperation(match[kBinOperation].str()),
             .operands = {Operand(match[kBinOp1].str()),
                          Operand(match[kBinOp2].str())},
             .destination = match[kBinDest].str()});
      } else if (match[kIsNotOp].matched) {
        instructions.push_back({.operation = Operation::kNot,
                                .operands = {Operand(match[kNotOp].str())},
                                .destination = match[kNotDest].str()});
      } else if (match[kIsAssignment].matched) {
        instructions.push_back(
            {.operation = Operation::kAssignment,
             .operands = {Operand(match[kAssignmentOp].str())},
             .destination = match[kAssignmentDest].str()});
      } else {
        std::cout << "No match found for: " << line << std::endl;
      }
    }
  }
  return instructions;
}

int main() {
  std::vector<Instruction> instructions = ParseInstructions();

  // If a wire has a known value, it is recorded here.
  absl::flat_hash_map<std::string, uint16_t> wires;

  // Repeat until all instructions are executed.
  while (!std::all_of(
      instructions.begin(), instructions.end(),
      [](const Instruction& instruction) { return instruction.executed; })) {
    // Try to execute each instruction.
    for (Instruction& instruction : instructions) {
      TryExecute(instruction, wires);
    }
  }
  std::cout << "Wire a: " << wires["a"] << std::endl;

  // If a wire has a known value, it is recorded here.
  absl::flat_hash_map<std::string, uint16_t> part_two_wires;

  // Mark the wire b with the value from a.
  part_two_wires["b"] = wires["a"];

  std::for_each(instructions.begin(), instructions.end(),
                [](Instruction& instruction) {
                  // Reset every instruction except the one that assigns b.
                  if (instruction.destination != "b") {
                    instruction.executed = false;
                  }
                });

  // Repeat until all instructions are executed.
  while (!std::all_of(
      instructions.begin(), instructions.end(),
      [](const Instruction& instruction) { return instruction.executed; })) {
    // Try to execute each instruction.
    for (Instruction& instruction : instructions) {
      TryExecute(instruction, part_two_wires);
    }
  }
  std::cout << "Part 2 wire a: " << part_two_wires["a"] << std::endl;

  return 0;
}
