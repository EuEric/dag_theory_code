#pragma once

#include "graph.h"
#include <unordered_set>
#include <vector>

using namespace std;

// Compute the volume (sum of WCET) of a set of vertices
int compute_volume(const unordered_set<int>& vertices, const vector<Task>& tasks);

// Compute the total length of a path in terms of WCET
int compute_length_path(const vector<int>& path, const vector<Task>& tasks);

// Compute the interference set of a single vertex
unordered_set<int> compute_interference_set(const Graph& G, const vector<int>& priority, int v);

// Compute the interference set of a set of vertices
unordered_set<int> compute_interference_set(const Graph& G, const vector<int>& priority, const vector<int>& vertices);

// Compute the response-time bound
float compute_rt_bound(
    const Graph& G,
    const vector<int>& priority,
    //const vector<Task>& tasks,
    const int nr_cores
);
