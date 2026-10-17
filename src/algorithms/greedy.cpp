
#include "algorithms.hpp"

#include <algorithm>

using namespace std;

Solution solveGreedy(const Problem& problem) {
    validateProblem(problem);

    Solution result;

    int n = static_cast<int>(problem.items.size());

    // Store item indices so we can sort without changing the input.
    vector<int> indices(n);

    for (int i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Sort items by value-to-weight ratio, highest first.
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

    int remainingCapacity = problem.capacity;

    // Select each item if it fits.
    for (int index : indices) {
        result.statesExplored++;

        const Item& item = problem.items[index];

        if (item.weight <= remainingCapacity) {
            result.selectedIndices.push_back(index);
            result.totalWeight += item.weight;
            result.totalValue += item.value;

            remainingCapacity -= item.weight;
        }
    }

    return result;
}
