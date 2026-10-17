
#include "algorithms.hpp"
#include <algorithm>

using namespace std;

int knapsack01Rec(
    const Problem& problem,
    int capacity,
    int n,
    vector<int>& selected
) {
    if (n == 0 || capacity == 0) {
        return 0;
    }

    const Item& item = problem.items[n - 1];

    // If the item is too heavy, skip it.
    if (item.weight > capacity) {
        return knapsack01Rec(
            problem, capacity, n - 1, selected
        );
    }

    // Try including the item.
    vector<int> includeItems = selected;
    includeItems.push_back(n - 1);

    int include = item.value + knapsack01Rec(
        problem,
        capacity - item.weight,
        n - 1,
        includeItems
    );

    // Try excluding the item.
    vector<int> excludeItems = selected;

    int exclude = knapsack01Rec(
        problem,
        capacity,
        n - 1,
        excludeItems
    );

    // Keep the items from the better choice.
    if (include > exclude) {
        selected = includeItems;
        return include;
    } else {
        selected = excludeItems;
        return exclude;
    }
}

Solution solveRecursive(const Problem& problem) {
    validateProblem(problem);

    Solution result;

    result.totalValue = knapsack01Rec(
        problem,
        problem.capacity,
        static_cast<int>(problem.items.size()),
        result.selectedIndices
    );

    for (int index : result.selectedIndices) {
        result.totalWeight += problem.items[index].weight;
    }

    return result;
}
