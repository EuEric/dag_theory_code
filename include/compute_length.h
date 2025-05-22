#pragma once

#include "graph.h"
#include <vector>

// Computes lf, lb, and l for each task and prints them.
void compute_length(
    //const std::vector<Task>& tasks,
                    const Graph& graph,
                    std::vector<int>& lf,  // Pass lf by reference
                    std::vector<int>& lb,  // Pass lb by reference
                    std::vector<int>& l    // Pass l by reference
                    );
