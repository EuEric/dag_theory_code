#include "topological_sort.h"
#include <queue>

using namespace std;

// Topological sort using Kahn's algorithm
vector<int> topological_sort(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<int> indegree(n, 0);
    for (const auto& neighbors : adj) {
        for (int v : neighbors) {
            indegree[v]++;
        }
    }

    queue<int> q;
    vector<int> topo_order;
    for (int i = 0; i < n; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
        }
    }

    while (!q.empty()) {
        int u = q.front(); q.pop();
        topo_order.push_back(u);
        for (int v : adj[u]) {
            if (--indegree[v] == 0) {
                q.push(v);
            }
        }
    }

    return topo_order;
}