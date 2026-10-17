
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// Represents one item that the drone can carry
struct Item {
    string name;
    int weight;
    int value;
};

// Represents the complete knapsack problem
struct Problem {
    vector<Item> items;
    int capacity;
};

// Represents the solution returned by an algorithm
struct Solution {
    vector<int> selectedIndices;
    int totalWeight = 0;
    int totalValue = 0;
    long long statesExplored = 0;
};

// Validates the input problem before solving it
inline void validateProblem(const Problem& problem) {

    if (problem.capacity < 0) {
        throw invalid_argument(
            "Drone capacity cannot be negative."
        );
    }

    for (const Item& item : problem.items) {

        if (item.weight <= 0) {
            throw invalid_argument(
                "Item weight must be positive."
            );
        }

        if (item.value < 0) {
            throw invalid_argument(
                "Item value cannot be negative."
            );
        }
    }
}
