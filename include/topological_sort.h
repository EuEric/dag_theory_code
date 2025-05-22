#pragma once

#include <vector>

// Returns a topological sort of the DAG using Kahn's algorithm.
std::vector<int> topological_sort(const std::vector<std::vector<int>>& adj);
