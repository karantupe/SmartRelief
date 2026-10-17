
#include "algorithms.hpp"

#include <nlohmann/json.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;
using namespace chrono;

int main() {
    try {
        // 1. Read the JSON request from standard input.
        json request;
        cin >> request;

        // 2. Read the drone capacity and selected algorithm.
        Problem problem;

        problem.capacity = request.at("capacity").get<int>();

        string algorithmName =
            request.at("algorithm").get<string>();

        // 3. Read all relief items.
        for (const auto& inputItem : request.at("items")) {
            Item item;

            item.name = inputItem.at("name").get<string>();
            item.weight = inputItem.at("weight").get<int>();
            item.value = inputItem.at("value").get<int>();

            problem.items.push_back(item);
        }

        // 4. Validate the problem.
        validateProblem(problem);

        // 5. Start measuring algorithm execution time.
        auto startTime = steady_clock::now();

        // 6. Run the requested algorithm.
        Solution solution;

        if (algorithmName == "recursive") {
            solution = solveRecursive(problem);
        }
        else if (algorithmName == "dp") {
            solution = solveDynamicProgramming(problem);
        }
        else if (algorithmName == "greedy") {
            solution = solveGreedy(problem);
        }
        else if (algorithmName == "branch_bound") {
            solution = solveBranchAndBound(problem);
        }
        else {
            throw invalid_argument(
                "Unknown algorithm. Use recursive, dp, greedy, or branch_bound."
            );
        }

        // 7. Stop measuring immediately after the algorithm finishes.
        auto endTime = steady_clock::now();

        double elapsedMs =
            duration<double, milli>(endTime - startTime).count();

        // 8. Build the list of selected items.
        json selectedItems = json::array();

        for (int index : solution.selectedIndices) {
            const Item& item = problem.items.at(index);

            selectedItems.push_back({
                {"index", index},
                {"name", item.name},
                {"weight", item.weight},
                {"value", item.value}
            });
        }

        // 9. Build the JSON response.
        json response = {
            {"success", true},
            {"algorithm", algorithmName},
            {"capacity", problem.capacity},
            {"totalWeight", solution.totalWeight},
            {"totalValue", solution.totalValue},
            {"statesExplored", solution.statesExplored},
            {"elapsedMs", elapsedMs},
            {"selectedItems", selectedItems}
        };

        // 10. Print JSON for Flask to read.
        cout << response.dump(2) << '\n';
    }
    catch (const exception& error) {
        // Return errors in JSON format.
        json response = {
            {"success", false},
            {"error", error.what()}
        };

        cout << response.dump(2) << '\n';
        return 1;
    }

    return 0;
}
