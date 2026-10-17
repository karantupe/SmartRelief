
#include "algorithms.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

namespace fs = filesystem;

// Stores the results of one benchmark run.
struct BenchmarkResult {
    int itemCount;
    string algorithmName;
    int totalValue;
    int totalWeight;
    int optimalValue;
    double medianTimeMs;
    long long statesExplored;
    double optimalityGap;
    bool feasible;
    bool consistent;
    bool optimal;
};

// Calculate the median execution time.
double calculateMedian(vector<double> times) {
    sort(times.begin(), times.end());

    int n = static_cast<int>(times.size());

    if (n == 0) {
        return 0.0;
    }

    if (n % 2 == 1) {
        return times[n / 2];
    }

    return (times[n / 2 - 1] + times[n / 2]) / 2.0;
}

// Verify selected indices, reported totals, and capacity.
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

        if (selected[index]) {
            return false;
        }

        selected[index] = true;

        const Item& item = problem.items[index];

        calculatedWeight += item.weight;
        calculatedValue += item.value;
    }

    return calculatedWeight == solution.totalWeight &&
           calculatedValue == solution.totalValue &&
           solution.totalWeight >= 0 &&
           solution.totalWeight <= problem.capacity;
}

// Run an algorithm several times and record its median runtime.
BenchmarkResult runBenchmark(
    int itemCount,
    const string& name,
    Solution (*algorithm)(const Problem&),
    const Problem& problem,
    int optimalValue,
    int repetitions
) {
    Solution solution;
    vector<double> times;

    for (int run = 0; run < repetitions; run++) {
        auto start = steady_clock::now();

        solution = algorithm(problem);

        auto end = steady_clock::now();

        double elapsedMs =
            duration<double, milli>(end - start).count();

        times.push_back(elapsedMs);
    }

    double medianTimeMs = calculateMedian(times);

    bool feasible =
        solution.totalWeight >= 0 &&
        solution.totalWeight <= problem.capacity;

    bool consistent = validateSolution(problem, solution);

    double optimalityGap = 0.0;

    if (optimalValue > 0) {
        optimalityGap =
            100.0 * (optimalValue - solution.totalValue)
            / optimalValue;
    }

    bool optimal =
        consistent && solution.totalValue == optimalValue;

    return {
        itemCount,
        name,
        solution.totalValue,
        solution.totalWeight,
        optimalValue,
        medianTimeMs,
        solution.statesExplored,
        optimalityGap,
        feasible,
        consistent,
        optimal
    };
}

// Generate the same problem whenever the same seed is used.
Problem generateProblem(int itemCount, unsigned int seed) {
    mt19937 generator(seed);

    uniform_int_distribution<int> weightDistribution(1, 15);
    uniform_int_distribution<int> valueDistribution(10, 100);

    Problem problem;
    problem.capacity = itemCount * 5;

    for (int i = 0; i < itemCount; i++) {
        Item item;

        item.name = "Item " + to_string(i + 1);
        item.weight = weightDistribution(generator);
        item.value = valueDistribution(generator);

        problem.items.push_back(item);
    }

    return problem;
}

int main() {
    vector<pair<string, Solution (*)(const Problem&)>> exactAlgorithms = {
        {"Recursive", solveRecursive},
        {"Branch and Bound", solveBranchAndBound}
    };

    vector<pair<string, Solution (*)(const Problem&)>> fastAlgorithms = {
        {"Dynamic Programming", solveDynamicProgramming},
        {"Greedy", solveGreedy}
    };

    // Exact search algorithms get smaller inputs initially.
    vector<int> exactSizes = {10, 15, 20};

    // DP and Greedy can be tested on larger inputs.
    vector<int> fastSizes = {10, 20, 50, 100, 200};

    const int repetitions = 5;

    fs::create_directories("benchmarks/results");

    ofstream csv("benchmarks/results/benchmark_results.csv");

    if (!csv.is_open()) {
        cerr << "Error: Could not open benchmark CSV file.\n";
        return 1;
    }

    csv << "item_count,algorithm,total_value,total_weight,capacity,"
        << "optimal_value,optimality_gap_percent,median_time_ms,"
        << "states_explored,feasible,consistent,optimal\n";

    cout << "========== SmartRelief Benchmark ==========\n";
    cout << "Median of " << repetitions << " runs per algorithm.\n\n";

    bool allValid = true;

    auto runGroup = [&](const vector<int>& sizes,
                        const vector<pair<string, Solution (*)(const Problem&)>>& group) {

        for (int itemCount : sizes) {
            Problem problem = generateProblem(
                itemCount,
                12345 + itemCount
            );

            // DP provides the reference optimum for this problem.
            Solution reference = solveDynamicProgramming(problem);

            if (!validateSolution(problem, reference)) {
                cerr << "DP reference failed validation for "
                     << itemCount << " items.\n";

                return false;
            }

            int optimalValue = reference.totalValue;

            for (const auto& algorithm : group) {
                BenchmarkResult result = runBenchmark(
                    itemCount,
                    algorithm.first,
                    algorithm.second,
                    problem,
                    optimalValue,
                    repetitions
                );

                bool valid = result.feasible && result.consistent;

                if (!valid) {
                    allValid = false;
                }

                cout << "Items: " << result.itemCount
                     << " | Algorithm: " << result.algorithmName
                     << " | Value: " << result.totalValue
                     << " | Optimum: " << result.optimalValue
                     << " | Gap: " << fixed << setprecision(2)
                     << result.optimalityGap << "%"
                     << " | Median: " << setprecision(6)
                     << result.medianTimeMs << " ms"
                     << " | Valid: " << (valid ? "Yes" : "No")
                     << '\n';

                csv << result.itemCount << ','
                    << result.algorithmName << ','
                    << result.totalValue << ','
                    << result.totalWeight << ','
                    << problem.capacity << ','
                    << result.optimalValue << ','
                    << setprecision(8) << result.optimalityGap << ','
                    << setprecision(10) << result.medianTimeMs << ','
                    << result.statesExplored << ','
                    << (result.feasible ? "true" : "false") << ','
                    << (result.consistent ? "true" : "false") << ','
                    << (result.optimal ? "true" : "false")
                    << '\n';
            }
        }

        return true;
    };

    bool success = runGroup(exactSizes, exactAlgorithms);

    if (success) {
        success = runGroup(fastSizes, fastAlgorithms);
    }

    csv.close();

    cout << "\nResults saved to "
         << "benchmarks/results/benchmark_results.csv\n";

    if (!success || !allValid) {
        cout << "Benchmark completed with validation errors.\n";
        return 1;
    }

    cout << "Benchmark completed successfully.\n";
    return 0;
}
