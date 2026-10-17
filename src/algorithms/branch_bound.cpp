
#include "algorithms.hpp"

#include <algorithm>

using namespace std;

struct Node {
    int level;
    int weight;
    int value;
    double bound;
    vector<int> selectedIndices;
};

// Calculate an optimistic upper bound for a node.
double calculateBound(
    const Problem& problem,
    const vector<int>& indices,
    int level,
    int currentWeight,
    int currentValue
) {
    int n = static_cast<int>(indices.size());

    if (currentWeight >= problem.capacity) {
        return currentValue;
    }

    double bound = currentValue;
    int totalWeight = currentWeight;

    // Fill the remaining capacity with whole items first.
    for (int i = level; i < n; i++) {
        const Item& item = problem.items[indices[i]];

        if (totalWeight + item.weight <= problem.capacity) {
            totalWeight += item.weight;
            bound += item.value;
        } else {
            // Allow a fraction only for the upper-bound estimate.
            int remaining = problem.capacity - totalWeight;

            bound +=
                static_cast<double>(remaining) *
                item.value / item.weight;

            break;
        }
    }

    return bound;
}

Solution solveBranchAndBound(const Problem& problem) {
    validateProblem(problem);

    Solution best;

    int n = static_cast<int>(problem.items.size());

    // Sort indices by value-to-weight ratio.
    vector<int> indices(n);

    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }

    sort(indices.begin(), indices.end(),
         [&](int a, int b) {
             double ratioA =
                 static_cast<double>(problem.items[a].value) /
                 problem.items[a].weight;

             double ratioB =
                 static_cast<double>(problem.items[b].value) /
                 problem.items[b].weight;

             return ratioA > ratioB;
         });

    vector<Node> pending;

    Node root;
    root.level = 0;
    root.weight = 0;
    root.value = 0;
    root.bound = calculateBound(
        problem, indices, 0, 0, 0
    );

    pending.push_back(root);

    while (!pending.empty()) {
        Node current = pending.back();
        pending.pop_back();

        best.statesExplored++;

        // No better solution can come from this branch.
        if (current.bound <= best.totalValue) {
            continue;
        }

        if (current.level == n) {
            continue;
        }

        int index = indices[current.level];
        const Item& item = problem.items[index];

        // Branch 1: Include the current item.
        if (current.weight + item.weight <= problem.capacity) {
            Node includeNode = current;

            includeNode.level++;
            includeNode.weight += item.weight;
            includeNode.value += item.value;
            includeNode.selectedIndices.push_back(index);

            if (includeNode.value > best.totalValue) {
                best.totalValue = includeNode.value;
                best.totalWeight = includeNode.weight;
                best.selectedIndices =
                    includeNode.selectedIndices;
            }

            includeNode.bound = calculateBound(
                problem,
                indices,
                includeNode.level,
                includeNode.weight,
                includeNode.value
            );

            if (includeNode.bound > best.totalValue) {
                pending.push_back(includeNode);
            }
        }

        // Branch 2: Exclude the current item.
        Node excludeNode = current;

        excludeNode.level++;

        excludeNode.bound = calculateBound(
            problem,
            indices,
            excludeNode.level,
            excludeNode.weight,
            excludeNode.value
        );

        if (excludeNode.bound > best.totalValue) {
            pending.push_back(excludeNode);
        }
    }

    return best;
}
