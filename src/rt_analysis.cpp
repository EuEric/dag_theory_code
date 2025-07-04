#include "graph.h"
#include "topological_sort.h"
#include <set>
#include <unordered_set>
#include <vector>
#include <iostream>

using namespace std;

// Compute the volume (sum of WCET) of a set of vertices
int compute_volume(const unordered_set<int>& vertices, const vector<Task>& tasks) {
    int volume = 0;
    for (int v : vertices) {
        volume += tasks[v].wcet;
    }
    return volume;
}

// Compute the total length of a path in terms of WCET
int compute_length_path(const vector<int>& path, const vector<Task>& tasks) {
    int length = 0;
    for(int v : path) {
        length += tasks[v].wcet;
    }
    return length;
}

// Compute the interference set of a vertex v
unordered_set<int> compute_interference_set(const Graph& G, const vector<int>& priority, int v) {
    int n = G.adj.size();
    
    unordered_set<int> ancestors = G.get_ancestors(v);
    unordered_set<int> descendants = G.get_descendants(v);

    unordered_set<int> interference_set;
    for (int u = 0; u < n; ++u) {
        if (u == v) continue;
        if (priority[u] < priority[v] &&
            !ancestors.count(u) &&
            !descendants.count(u)) {
            interference_set.insert(u);
        }
    }

    return interference_set;
}

// Compute the interference set of a set of vertices
unordered_set<int> compute_interference_set(
    const Graph& G,
    const vector<int>& priority,
    const vector<int>& vertices
) {
    unordered_set<int> result;

    for (int v : vertices) {
        unordered_set<int> interference = compute_interference_set(G, priority, v);
        result.insert(interference.begin(), interference.end());
    }

    return result;
}

// Compute the response-time bound
float compute_rt_bound(
    const Graph& G,
    const vector<int>& priority,
    //const vector<Task>& tasks,
    const int nr_cores
) {
    vector<int> topo_order = topological_sort(G.adj);

    cout << "TOPOLOGICAL ORDER: ";
    for (int v : topo_order) {
        cout << v << " ";
    }
    cout << "\n";

    int n = G.tasks.size();

    // Initialize paths for each vertex
    vector<vector<int>> paths(n);
    // Start path from source           
    paths[topo_order[0]].push_back(topo_order[0]);  
    
    for (auto it = topo_order.begin(); it != topo_order.end(); ++it) {
        int v = *it;
        if (it == topo_order.begin()) continue;
    
        vector<int> predecessors = G.predecessors(v);
        int max_value = 0;
        int max_vertex = -1;
        float cur_value = 0;
    
        for (int pred : predecessors) {
            cur_value = compute_length_path(paths[pred], G.tasks) + G.tasks[v].wcet;
    
            unordered_set<int> result;
    
            unordered_set<int> interference_path = compute_interference_set(G, priority, paths[pred]);
            // cout << "Interference set of path made from vertices: ";
            // for(int vertex : paths[pred]) {
            //     cout << vertex << " ";
            // }
            // cout << " - IS: ";
            // for (int vertex: interference_path) {
            //     cout << vertex << " ";
            // }
            // cout << "\n";

            result.insert(interference_path.begin(), interference_path.end());
    
            unordered_set<int> interference_v = compute_interference_set(G, priority, v);
            // cout << "Interference set of vertex " << v << " is: ";
            // for (int vertex : interference_v) {
            //     cout << vertex << " ";
            // }
            // cout << "\n";
            result.insert(interference_v.begin(), interference_v.end());
    
            int volume = compute_volume(result, G.tasks);
            cur_value += volume / nr_cores;
    
            if (cur_value > max_value) {
                max_value = cur_value;
                max_vertex = pred;
            }
        }
    
        // Update path
        paths[v] = paths[max_vertex];
        paths[v].push_back(v);
    }

    // Return R(path sink)
    int sink = topo_order.back();
    cout << "Path to sink is: ";
    for (int vertex : paths[sink]) {
        cout << vertex << " ";
    }
    cout << "\n";
    int length_path_to_sink = compute_length_path(paths[sink], G.tasks);

    unordered_set<int> interference_path_to_sink = compute_interference_set(G, priority, paths[sink]);
    cout << "Interference set of path to sink is: ";
    for (int vertex : interference_path_to_sink) {
        cout << vertex << " ";
    }
    cout << "\n";
    int volume = compute_volume(compute_interference_set(G, priority, paths[sink]), G.tasks);
    cout << "Volume of the interference set is: " << volume;

    return length_path_to_sink + volume / nr_cores;
    
}
    
