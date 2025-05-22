#pragma once

#include "graph.h"
#include <vector>


void assign_priority(
    Graph G,                          // Copy of graph
    //const std::vector<Task>& tasks,   // Task metadata
    const std::vector<int>& l,        // Computed l(v)
    const std::vector<int>& lb,       // Computed lb(v)
    std::vector<int>& priority,       // Output priorities
    int& p                            // Priority counter (starting at 0)
);

// Propagates indirect priorities using DFS
void dfs_propagate(
    int source,                                // Vertex from which we start assigning priorities for indirect edges
    unordered_set<int>& visited,               // Visited set
    const vector<vector<int>>& indirect_adj,   // Indirect edge adjancency representation
    vector<int>& priority                      // Priority array
);

// Assigns (propagates) priorities for 'indirect' edges to all connected components
void assign_indirect_priority(
    const vector<vector<int>>& indirect_adj,   // Indirect edge adjancency representation
    vector<int>& priority                      // Priority array
);