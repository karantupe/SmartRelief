
#include "algorithms.hpp"

#include <algorithm>

using namespace std;

Solution solveDynamicProgramming(const Problem& problem) {
    validateProblem(problem);

    int n = static_cast<int>(problem.items.size());
    int capacity = problem.capacity;

    // dp[i][w] stores the best value using the first i items.
    vector<vector<int>> dp(
        n + 1,
        vector<int>(capacity + 1, 0)
    );

    // Fill the DP table.
    for (int i = 1; i <= n; i++) {
        const Item& item = problem.items[i - 1];

        for (int w = 0; w <= capacity; w++) {
            // Choice 1: Exclude the current item.
            dp[i][w] = dp[i - 1][w];

            // Choice 2: Include the current item, if it fits.
            if (item.weight <= w) {
                dp[i][w] = max(
                    dp[i][w],
                    item.value + dp[i - 1][w - item.weight]
                );
            }
        }
    }

    Solution result;

    result.totalValue = dp[n][capacity];

    // Trace backwards through the table to find selected items.
    int w = capacity;

    for (int i = n; i > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            int index = i - 1;

            result.selectedIndices.push_back(index);
            result.totalWeight += problem.items[index].weight;

            w -= problem.items[index].weight;
        }
    }

    return result;
}
