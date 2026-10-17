
cmake_minimum_required(VERSION 3.16)

project(SmartRelief LANGUAGES CXX)

find_package(nlohmann_json 3.2.0 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SmartRelief
    src/main.cpp
    src/algorithms/recursive.cpp
    src/algorithms/dp.cpp
    src/algorithms/greedy.cpp
    src/algorithms/branch_bound.cpp
)

target_include_directories(SmartRelief PRIVATE include)

add_executable(SmartReliefBenchmark
    src/benchmark.cpp
    src/algorithms/recursive.cpp
    src/algorithms/dp.cpp
    src/algorithms/greedy.cpp
    src/algorithms/branch_bound.cpp
)

target_include_directories(SmartReliefBenchmark PRIVATE include)


enable_testing()

add_executable(SmartReliefTests
    tests/test_algorithms.cpp
    src/algorithms/recursive.cpp
    src/algorithms/dp.cpp
    src/algorithms/greedy.cpp
    src/algorithms/branch_bound.cpp
)

target_include_directories(SmartReliefTests PRIVATE include)

add_test(
    NAME SmartReliefAlgorithmTests
    COMMAND SmartReliefTests
)


add_executable(SmartReliefEngine
    app/engine_cli.cpp
    src/algorithms/recursive.cpp
    src/algorithms/dp.cpp
    src/algorithms/greedy.cpp
    src/algorithms/branch_bound.cpp
)

target_include_directories(SmartReliefEngine PRIVATE include)

target_link_libraries(SmartReliefEngine PRIVATE nlohmann_json::nlohmann_json)
