#include "compute_length.h"
#include "topological_sort.h"
#include <queue>
#include <iostream>

using namespace std;

// Compute l(v), lf(v), lb(v) for each task
void compute_length(
    // const vector<Task>& tasks,
    const Graph& graph,
    vector<int>& lf,
    vector<int>& lb,
    vector<int>& l
) {
    vector<int> topo_order = topological_sort(graph.adj);

    // Forward pass from source to sink
    for (auto it = topo_order.begin(); it != topo_order.end(); ++it) {
        int v = *it;
        int max_pred = 0;
        for (int u : graph.reverse_adj[v]) {
            max_pred = max(max_pred, lf[u]);
        }
        lf[v] = graph.tasks[v].wcet + max_pred;
    }

    // Backward pass from sink to source
    for (auto it = topo_order.rbegin(); it != topo_order.rend(); ++it) {
        int v = *it;
        int max_succ = 0;
        for (int u : graph.adj[v]) {
            max_succ = max(max_succ, lb[u]);
        }
        lb[v] = graph.tasks[v].wcet + max_succ;
    }

    // Combined length
    for (int v : topo_order) {
        l[v] = lf[v] + lb[v] - graph.tasks[v].wcet;
    }
}
