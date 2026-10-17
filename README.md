# SmartRelief 
SmartRelief is a C++20-based optimization project that helps select the most valuable combination of emergency supplies within a drone's limited carrying capacity.

The project implements four algorithms for the **0/1 Knapsack Problem** and integrates them with a Flask backend and an interactive web dashboard to visualize optimization results and compare algorithm performance.

## Features

- Four optimization algorithms: Recursive Backtracking, Dynamic Programming, Greedy, and Branch and Bound.
- Interactive dashboard for configuring drone capacity and relief items.
- Flask backend connected to a C++ optimization engine.
- Visualization of selected items, payload utilization, and algorithm results.
- Benchmarking tools for measuring algorithm performance.
- Automated tests for validating algorithm behavior.

## Project Structure

```text
SmartRelief/
├── app/
│   ├── static/
│   │   ├── css/             # Dashboard stylesheets
│   │   └── js/              # Frontend JavaScript
│   ├── templates/           # HTML templates
│   └── engine_cli.cpp       # C++ engine JSON interface
│
├── include/
│   ├── algorithms.hpp       # Algorithm declarations
│   └── problem.hpp          # Problem and data definitions
│
├── src/
│   ├── algorithms/
│   │   ├── recursive.cpp    # Recursive Backtracking
│   │   ├── dp.cpp            # Dynamic Programming
│   │   ├── greedy.cpp        # Greedy algorithm
│   │   └── branch_bound.cpp  # Branch and Bound
│   ├── benchmark.cpp         # Benchmarking implementation
│   └── main.cpp              # Main C++ entry point
│
├── tests/
│   └── test_algorithms.cpp   # Algorithm test cases
│
├── benchmarks/
│   └── results/
│       └── benchmark_results.csv
│
├── CMakeLists.txt            # C++ build configuration
└── README.md
```

## Installation and Setup

Follow these steps to build and run SmartRelief locally.

### 1. Prerequisites

Make sure the following tools are installed:

- **Git** — to clone the repository.
- **C++20 compiler** — GCC 10+ or a compatible compiler.
- **CMake** — to configure and build the C++ project.
- **Python 3** — to run the Flask backend.
- **pip** — to install Python dependencies.
- **nlohmann/json** — C++ JSON library used by the engine.

On Ubuntu or Debian-based Linux distributions, install the required system packages:

```bash
sudo apt update
sudo apt install git build-essential cmake python3 python3-pip python3-venv nlohmann-json3-dev
```

Verify the installations:

```bash
git --version
g++ --version
cmake --version
python3 --version
```

### 2. Clone the Repository

Open a terminal and run:

```bash
git clone https://github.com/karantupe/SmartRelief.git
cd SmartRelief
```

### 3. Build the C++ Project

Configure the project with CMake:

```bash
cmake -S . -B build
```

Compile the project:

```bash
cmake --build build
```

This creates the compiled targets defined in `CMakeLists.txt`, including the optimization engine and other configured executables.

### 4. Run the C++ Optimization Engine

On Linux, test the engine using a JSON input:

```bash
echo '{"algorithm":"dp","capacity":10,"items":[{"name":"Medical Kit","weight":2,"value":20},{"name":"Food Pack","weight":3,"value":30},{"name":"Water Supply","weight":4,"value":50},{"name":"First Aid Box","weight":5,"value":60},{"name":"Emergency Tent","weight":9,"value":90}]}' | ./build/SmartReliefEngine
```

The engine should return a JSON response containing the optimization result and relevant performance information.

For this example, the optimal total value is `110`, with a total selected weight of `9`.

### 5. Set Up the Python Environment

Create a virtual environment in the project root:

```bash
python3 -m venv .venv
```

Activate it on Linux or macOS:

```bash
source .venv/bin/activate
```

On Windows PowerShell:

```powershell
.venv\Scripts\Activate.ps1
```

Install the required Python dependencies.

If the repository contains a `requirements.txt` file, run:

```bash
pip install -r requirements.txt
```

Otherwise, install Flask and any other packages required by the application:

```bash
pip install Flask
```

### 6. Start the Web Application

Run the Flask application using the Python entry-point file in your project.

For example, if the entry point is `app.py` in the project root:

```bash
python app.py
```

If the entry point is named differently or located inside another directory, use the corresponding path.

Once Flask starts successfully, open the local address printed in the terminal, commonly:

```text
http://127.0.0.1:5000
```

You should now be able to interact with the SmartRelief dashboard.

### 7. Run the Tests

After building the project, execute the test target configured in CMake:

```bash
ctest --test-dir build --output-on-failure
```

Alternatively, run the test executable directly if your build configuration generates it:

```bash
./build/SmartReliefTests
```

These commands help verify that the algorithm implementations behave as expected.

### 8. Run the Benchmark

If the benchmark executable is configured as `SmartReliefBenchmark`, run:

```bash
./build/SmartReliefBenchmark
```

The benchmark program generates performance measurements according to its implementation and configuration. Check the terminal output and the `benchmarks/results/` directory for the generated results.

## Notes

- The build commands above target Linux. Windows users can use a compatible compiler and CMake environment, such as Visual Studio with CMake or WSL.
- The executable names and output paths depend on the targets defined in `CMakeLists.txt`.
- The Flask startup command must match the actual entry point in the repository.
- The `.venv/` and `build/` directories are generated locally and should generally not be committed to Git.
- SmartRelief currently models the single-constraint 0/1 Knapsack Problem. Additional constraints, such as battery range and multiple delivery destinations, are potential future extensions.

---

**Author:** [Karan Tupe](https://github.com/karantupe)  
**Repository:** [github.com/karantupe/SmartRelief](https://github.com/karantupe/SmartRelief)