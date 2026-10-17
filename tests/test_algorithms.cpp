
#include "algorithms.hpp"
#include<random>
#include <iostream>
#include <string>
#include <vector>
#include<stdexcept>

using namespace std;

bool validateSolution(
    const Problem& problem,
    const Solution& solution
) {
    int calculatedWeight = 0;
    int calculatedValue = 0;
    vector<bool> selected(problem.items.size(), false);

    for (int index : solution.selectedIndices) {
        if (index < 0 ||
            index >= static_cast<int>(problem.items.size())) {
            return false;
        }

        // An item must not be selected twice.
        if (selected[index]) {
            return false;
        }

        selected[index] = true;

        calculatedWeight += problem.items[index].weight;
        calculatedValue += problem.items[index].value;
    }

    return calculatedWeight == solution.totalWeight &&
           calculatedValue == solution.totalValue &&
           solution.totalWeight <= problem.capacity;
}

bool testAlgorithm(
    const string& name,
    Solution (*algorithm)(const Problem&),
    const Problem& problem,
    int expectedOptimalValue
) {
    Solution solution = algorithm(problem);

    bool valid = validateSolution(problem, solution);
    bool optimal = solution.totalValue == expectedOptimalValue;

    cout << name << ": ";

    if (valid && optimal) {
        cout << "PASS\n";
        return true;
    }

    cout << "FAIL";

    if (!valid) {
        cout << " - invalid solution";
    }

    if (!optimal) {
        cout << " - expected value " << expectedOptimalValue
             << ", got " << solution.totalValue;
    }

    cout << '\n';
    return false;
}

int main() {
    int passed = 0;
    int total = 0;

    vector<pair<string, Solution (*)(const Problem&)>> exactAlgorithms = {
        {"Recursive Backtracking", solveRecursive},
        {"Dynamic Programming", solveDynamicProgramming},
        {"Branch and Bound", solveBranchAndBound}
    };

    // Test 1: Standard example.
    Problem standardProblem{
        {
            {"Medical Kit", 2, 20},
            {"Food Pack", 3, 30},
            {"Water Supply", 4, 50},
            {"First Aid Box", 5, 60},
            {"Emergency Tent", 9, 90}
        },
        10
    };

    cout << "\nTest 1: Standard problem\n";

    for (const auto& algorithm : exactAlgorithms) {
        total++;
        if (testAlgorithm(
                algorithm.first,
                algorithm.second,
                standardProblem,
                110)) {
            passed++;
        }
    }

    total++;
    if (testAlgorithm(
            "Greedy",
            solveGreedy,
            standardProblem,
            110)) {
        passed++;
    }

    // Test 2: Zero capacity.
    Problem zeroCapacityProblem{
        {
            {"Medical Kit", 2, 20},
            {"Food Pack", 3, 30}
        },
        0
    };

    cout << "\nTest 2: Zero capacity\n";

    for (const auto& algorithm : exactAlgorithms) {
        total++;
        if (testAlgorithm(
                algorithm.first,
                algorithm.second,
                zeroCapacityProblem,
                0)) {
            passed++;
        }
    }

    total++;
    if (testAlgorithm(
            "Greedy",
            solveGreedy,
            zeroCapacityProblem,
            0)) {
        passed++;
    }

    // Test 3: No items.
    Problem emptyProblem{{}, 10};

    cout << "\nTest 3: No items\n";

    for (const auto& algorithm : exactAlgorithms) {
        total++;
        if (testAlgorithm(
                algorithm.first,
                algorithm.second,
                emptyProblem,
                0)) {
            passed++;
        }
    }

    total++;
    if (testAlgorithm(
            "Greedy",
            solveGreedy,
            emptyProblem,
            0)) {
        passed++;
    }

    // Test 4: Every item is heavier than the capacity.
    Problem oversizedItemsProblem{
        {
            {"Heavy Kit", 12, 100},
            {"Heavy Food Box", 15, 120}
        },
        5
    };

    cout << "\nTest 4: Items exceed capacity\n";

    for (const auto& algorithm : exactAlgorithms) {
        total++;
        if (testAlgorithm(
                algorithm.first,
                algorithm.second,
                oversizedItemsProblem,
                0)) {
            passed++;
        }
    }

    total++;
    if (testAlgorithm(
            "Greedy",
            solveGreedy,
            oversizedItemsProblem,
            0)) {
        passed++;
    }

    // Test 5: Greedy is not always optimal.
    Problem greedyFailureProblem{
        {
            {"Item A", 10, 60},
            {"Item B", 20, 100},
            {"Item C", 30, 120}
        },
        50
    };

    cout << "\nTest 5: Greedy suboptimal case\n";

    for (const auto& algorithm : exactAlgorithms) {
        total++;
        if (testAlgorithm(
                algorithm.first,
                algorithm.second,
                greedyFailureProblem,
                220)) {
            passed++;
        }
    }

    total++;
    Solution greedyResult = solveGreedy(greedyFailureProblem);

    bool greedyValid = validateSolution(
        greedyFailureProblem,
        greedyResult
    );

    // Greedy selects A and B, reaching value 160.
// The optimal solution selects B and C, reaching value 220.
    bool greedyExpected = greedyResult.totalValue == 160;

    cout << "Greedy: ";

    if (greedyValid && greedyExpected) {
        cout << "PASS (correctly demonstrates a suboptimal result)\n";
        passed++;
    } else {
        cout << "FAIL - expected feasible value 160, got "
             << greedyResult.totalValue << '\n';
    }

    // Test 6: Randomized cross-validation
    cout << "\nTest 6: Randomized cross-validation\n";

    mt19937 generator(2026);
    uniform_int_distribution<int> countDistribution(1, 12);
    uniform_int_distribution<int> weightDistribution(1, 15);
    uniform_int_distribution<int> valueDistribution(0, 100);
    uniform_int_distribution<int> capacityDistribution(0, 40);

    bool randomTestsPassed = true;
    const int numberOfCases = 100;

    for (int test = 1; test <= numberOfCases; test++) {
        Problem problem;
        int count = countDistribution(generator);
        problem.capacity = capacityDistribution(generator);

        for (int i = 0; i < count; i++) {
            Item item;
            item.name = "Item " + to_string(i + 1);
            item.weight = weightDistribution(generator);
            item.value = valueDistribution(generator);
            problem.items.push_back(item);
        }

        Solution recursive = solveRecursive(problem);
        Solution dp = solveDynamicProgramming(problem);
        Solution branchBound = solveBranchAndBound(problem);
        Solution greedy = solveGreedy(problem);

        bool valid =
            validateSolution(problem, recursive) &&
            validateSolution(problem, dp) &&
            validateSolution(problem, branchBound) &&
            validateSolution(problem, greedy);

        bool exactResultsAgree =
            recursive.totalValue == dp.totalValue &&
            dp.totalValue == branchBound.totalValue;

        if (!valid || !exactResultsAgree) {
            cout << "FAIL on randomized case " << test << '\n';
            cout << "Capacity: " << problem.capacity << '\n';
            cout << "Recursive: " << recursive.totalValue << '\n';
            cout << "DP: " << dp.totalValue << '\n';
            cout << "Branch and Bound: "
                 << branchBound.totalValue << '\n';
            cout << "Greedy: " << greedy.totalValue << '\n';

            randomTestsPassed = false;
            break;
        }
    }

    total++;
    if (randomTestsPassed) {
        cout << "PASS: all " << numberOfCases
             << " randomized cases\n";
        passed++;
    }








    cout << "\n============================\n";
    cout << "Tests passed: " << passed << " / " << total << '\n';

    if (passed == total) {
        cout << "All tests passed!\n";
        return 0;
    }

    cout << "Some tests failed.\n";
    return 1;
}
