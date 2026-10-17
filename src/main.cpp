
#include "algorithms.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

int main() {
    Problem problem{
        {
            {"Medical Kit", 2, 20},
            {"Food Pack", 3, 30},
            {"Water Supply", 4, 50},
            {"First Aid Box", 5, 60},
            {"Emergency Tent", 9, 90}
        },
        10
    };

    cout << "====================================\n";
    cout << "          SMARTRELIEF\n";
    cout << "====================================\n";
    cout << "Drone capacity: " << problem.capacity << " kg\n";
    cout << "Number of items: " << problem.items.size() << "\n\n";

    // Store the names and functions of all algorithms.
    vector<pair<string, Solution (*)(const Problem&)>> algorithms = {
        {"Recursive Backtracking", solveRecursive},
        {"Dynamic Programming", solveDynamicProgramming},
        {"Greedy", solveGreedy},
        {"Branch and Bound", solveBranchAndBound}
    };

    for (const auto& algorithm : algorithms) {
        auto start = steady_clock::now();

        Solution result = algorithm.second(problem);

        auto end = steady_clock::now();

        double timeTaken =
            duration<double, milli>(end - start).count();

        cout << "------------------------------------\n";
        cout << "Algorithm: " << algorithm.first << "\n";
        cout << "Selected items:\n";

        for (int index : result.selectedIndices) {
            const Item& item = problem.items[index];

            cout << "  - " << item.name
                 << " | Weight: " << item.weight << " kg"
                 << " | Value: " << item.value << '\n';
        }

        cout << "Total weight: " << result.totalWeight << " kg\n";
        cout << "Maximum value: " << result.totalValue << '\n';
        cout << "States explored: " << result.statesExplored << '\n';

        cout << fixed << setprecision(6);
        cout << "Execution time: " << timeTaken << " ms\n\n";
    }

    cout << "====================================\n";

    return 0;
}
