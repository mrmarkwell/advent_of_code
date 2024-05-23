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
*/

#include <iostream>
#include <optional>
#include <ostream>
#include <regex>
#include <stack>
#include <string>
#include <vector>

#include "utils/utils.h"

struct Node {
  uint16_t value = 0;
  std::string name;
  std::vector<Node*> neighbors;
  std::string instruction;
};

class Graph {
 public:
  std::vector<Node> nodes;

  void AddNode(Node node) { nodes.push_back(node); }

  void AddEdges(std::string from, std::string to) {
    Node& from_node =
        *std::find_if(nodes.begin(), nodes.end(),
                      [from](const Node& node) { return node.name == from; });
    Node& to_node =
        *std::find_if(nodes.begin(), nodes.end(),
                      [to](const Node& node) { return node.name == to; });

    from_node.neighbors.push_back(&to_node);
  }

  std::vector<std::string> StartTopologicalSort() {
    std::unordered_map<std::string, bool> visited;
    std::stack<std::string> stack;
    std::vector<std::string> result;

    for (const auto& node : nodes) {
      visited[node.name] = false;
    }

    for (const auto& node : nodes) {
      if (!visited[node.name]) {
        TopologicalSort(node, visited, stack);
      }
    }

    while (!stack.empty()) {
      result.push_back(stack.top());
      stack.pop();
    }

    std::reverse(result.begin(), result.end());
    return result;
  }

  void TopologicalSort(const Node& node,
                       std::unordered_map<std::string, bool>& visited,
                       std::stack<std::string>& stack) {
    visited[node.name] = true;

    for (const auto& neighbor : node.neighbors) {
      if (!visited[neighbor->name]) {
        TopologicalSort(*neighbor, visited, stack);
      }
    }

    stack.push(node.name);
  }

  void ComputeNode(std::string name) {
    Node* node = nullptr;
    // Gets a pointer to our node.
    for (Node& temp_node : nodes) {
      if (temp_node.name == name) {
        node = &temp_node;
      }
    }
    // If the node operation is the NOT operation.
    if (node->instruction.starts_with("NOT")) {
      node->value = ~node->neighbors[0]->value;

    }
    // If the node operation is the AND operation.
    else if (node->instruction.find("AND") != std::string::npos) {
      // If the node has 2 neighbors, apply the operation on those 2.
      if (node->neighbors.size() == 2) {
        node->value = node->neighbors[0]->value & node->neighbors[1]->value;
      }
      // With our input, we always have at least 1 neighbor which means we just
      // have to search for the other constant in the instruction.
      else {
        node->value = node->neighbors[0]->value & 1;
      }
    }
    // If the node operation is the OR operation.
    else if (node->instruction.find("OR") != std::string::npos) {
      node->value = node->neighbors[0]->value | node->neighbors[1]->value;
    }
    // If the node operation is the LSHIFT operation.
    else if (node->instruction.find("LSHIFT") != std::string::npos) {
      std::regex number_pattern("\\d+");
      std::smatch match;
      std::regex_search(node->instruction, match, number_pattern);

      node->value = node->neighbors[0]->value << std::stoi(match[0]);
    }
    // If the node operation is the RSHIFT operation.
    else if (node->instruction.find("RSHIFT") != std::string::npos) {
      std::regex number_pattern("\\d+");
      std::smatch match;
      std::regex_search(node->instruction, match, number_pattern);

      node->value = node->neighbors[0]->value >> std::stoi(match[0]);
    }
    // The default operation is an assignment operation.
    else {
      // If we have a neighbor, assign the neighbors value to this node
      if (node->neighbors.size() == 1) {
        node->value = node->neighbors[0]->value;
        // Search for a constant value in the instruction and assign that value
        // to the node.
      } else {
        std::regex number_pattern("\\d+");
        std::smatch match;
        std::regex_search(node->instruction, match, number_pattern);

        node->value = std::stoi(match[0]);
      }
    }
  }
};

Graph BuildGraph(const std::string& input) {
  Graph graph;
  std::smatch match;
  std::string::const_iterator searchStart(input.cbegin());
  std::regex re(R"((.*) -> (\w+))");

  while (std::regex_search(searchStart, input.cend(), match, re)) {
    graph.AddNode({.name = match[2], .instruction = match[1]});

    searchStart = match.suffix().first;
  }

  for (const auto& node : graph.nodes) {
    std::string::const_iterator instruction_search_start(
        node.instruction.cbegin());

    std::regex re2(
        R"(\b(?!AND\b)(?!OR\b)(?!NOT\b)(?!RSHIFT\b)(?!LSHIFT\b)[a-z]+\b)");
    while (std::regex_search(instruction_search_start, node.instruction.cend(),
                             match, re2)) {
      graph.AddEdges(node.name, match[0].str());

      instruction_search_start = match.suffix().first;
    }
  }

  return graph;
}

int main() {
  std::string input = aoc::ReadFileToString("./2015/day7.txt");
  Graph graph = BuildGraph(input);
  std::vector<std::string> build_order = graph.StartTopologicalSort();

  for (auto& name : build_order) {
    graph.ComputeNode(name);
  }
  // std ::cout << "\n\n\n\n\n\n\n\n\n\n\n";
  // for (auto i : graph.nodes) {
  //   std::cout << i.name << ": " << i.value << std::endl;
  // }

  Node& nodeA =
      *std::find_if(graph.nodes.begin(), graph.nodes.end(),
                    [](const Node& node) { return node.name == "a"; });

  std::cout << "A node: " << nodeA.value << std::endl;

  build_order.erase(build_order.begin());
  Node& nodeB =
      *std::find_if(graph.nodes.begin(), graph.nodes.end(),
                    [](const Node& node) { return node.name == "b"; });

  nodeB.value = nodeA.value;

  for (auto& name : build_order) {
    graph.ComputeNode(name);
  }

  std::cout << "New A: " << nodeA.value << std::endl;

  return 0;
}