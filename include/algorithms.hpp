
#pragma once

#include "problem.hpp"

using namespace std;

// 1. Recursive Backtracking
Solution solveRecursive(const Problem& problem);

// 2. Dynamic Programming
Solution solveDynamicProgramming(const Problem& problem);

// 3. Greedy Algorithm
Solution solveGreedy(const Problem& problem);

// 4. Branch and Bound
Solution solveBranchAndBound(const Problem& problem);
